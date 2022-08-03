//************************************************
//  Name    : MAX2871_Load_Word_115200_5.ino
//  Author  : WN2A and Mark Stanley
//  Date    : 21 May 2021
//  Version : 1.0
//  Notes   : Code for using MAX2871 Synth (1 Register at a time)
//  Use with FreeBasic Program MAX2871_Command_8.bas or higher
//************************************************
#include <SPI.h>
#include "max2871.h"

uint32_t z;

const int LOCK_PIN      =  3;   // MAX2871 Lock Detect
const int RF_EN_PIN     =  5;   // MAX2871 RF_EN
const int STROBE_PIN    = 11;   // MAX2871 STROBE
const int RF_EN_SWX_PIN = 12;   // Arduino RF enable signal. H=Enable
const int PLL_MUX_PIN   = A0;
const int CLOCK_PIN     = A1;   // MAX2871 SCLK
const int DATA_PIN      = A2;   // MAX2871 DATA
const int LO_SEL_PIN    = A3;   // MAX2871 LE

MAX2871_LO LO = MAX2871_LO();

// When the mux pin is configured for Digital Lock Detect output
// we can read the status of the pin from here.
void MuxTest() {
  char* LOStatus = " LO UNLOCKED";

  if (digitalRead(PLL_MUX_PIN)) {
    LOStatus = " LO LOCKED";
  }
  
  Serial.println(LOStatus);
}


/* IAW Manufacturer's PDF document "MAX2871 - 23.5MHz to 6000MHz Fractional/Integer-N Synthesizer/VCO"
   pg. 13 4-Wire Serial Interface during initialization there should be a 20mS delay after programming
   register 5.                                                  Document Version: 19-7106; Rev 4; 6/20
*/
void initialize_LO(uint8_t selectPin, bool initialize) {
  spiWriteLO(LO.Curr.Reg[5], selectPin);    // First we program LO Register 5
  if (initialize) {
    delay(20);  // Only if it's our first time must we wait 20 mSec
  }
  for (int x = 4; x >= 0; x--) {
    spiWriteLO(LO.Curr.Reg[x], selectPin); // Program remaining registers
  }
  delay(1);                               // Short delay before reading Register 6
  MuxTest();                              // Check if LO is locked by reading the Mux pin
  spiWriteLO(LO.Curr.Reg[6], selectPin);  // Tri-stating the mux output disables LO2 lock detect
}

// Program a single register of the selected LO by sending and latching 4 bytes
void spiWriteLO(uint32_t reg, uint8_t selectPin) {
  digitalWrite(LO_SEL_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, reg >> 24);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, reg >> 16);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, reg >> 8);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, reg);
  delayMicroseconds(10);
  digitalWrite(LO_SEL_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(LO_SEL_PIN, LOW);
}

//Test LOCK_PIN.RF_EN_SWX_PIN, drive GRN light
void Status() {
  int stat = 0;

  if (digitalRead(RF_EN_SWX_PIN) == HIGH ) {
    stat = 1;
    digitalWrite(RF_EN_PIN, HIGH);
  } else {
    digitalWrite(RF_EN_PIN, LOW);
  }

  if (digitalRead(LOCK_PIN) == HIGH ) {
    stat = stat + 2;
  }
  if (stat == 3 ) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  // stat=0 unlock,RF off; 1= unlock,RF on; 2=lock, RF off; 3=lock,RF on
}

bool DONE = false;
int addr;
char* message = "MAX2871_Load_Word_115200_5.ino 21 May 2021";

void setup() {
  // set pins to output because they are addressed in the main loop
  pinMode(LOCK_PIN, INPUT);
  pinMode(RF_EN_PIN, OUTPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RF_EN_SWX_PIN, INPUT_PULLUP);
  pinMode(PLL_MUX_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LO_SEL_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RF_EN_PIN, LOW);         // Initially turn off MAX2871 RF Output
  digitalWrite(LED_BUILTIN, HIGH);  // Initially, turn off Lock Detect Indicator
  digitalWrite(LO_SEL_PIN, LOW);   // Latch must start LOW
  digitalWrite(STROBE_PIN, HIGH);       // Strobe HIGH so you know it's ready before starting
  Serial.begin(2000000);
}

void loop() {
  if (!DONE) {
    initialize_LO(LO_SEL_PIN, true);
    initialize_LO(LO_SEL_PIN, false);
    Serial.println(" !DONE");
    Serial.println(message);
    Serial.println(" Stepped over println message");
    DONE = true;
  }
  if (Serial.available()) {
    z = Serial.parseInt();
  }
  
  if (z > 0) {
    addr = z & 0x00000007;    // First 3 bits are the chip Register address
    if (LO.Curr.Reg[addr] != z) {
      Serial.println("Reg[addr] != z");
      LO.Curr.Reg[addr] = z;
      spiWriteLO(LO.Curr.Reg[addr], LO_SEL_PIN);
    }
    Status();
  }
  z = 0;
}
