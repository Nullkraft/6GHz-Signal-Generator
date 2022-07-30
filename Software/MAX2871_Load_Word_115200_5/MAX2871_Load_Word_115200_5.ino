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

const int lockPin     =  3;   // MAX2871 Lock Detect
const int RF_En       =  5;   // MAX2871 RF_EN
const int strobe      = 11;   // MAX2871 STROBE
const int RF_en_swx   = 12;   // Arduino RF enable signal. H=Enable
const int PLL_MUX     = A0;
const int clockPin    = A1;   // MAX2871 SCLK
const int dataPin     = A2;   // MAX2871 DATA
const int MAX2871_SEL = A3;   // MAX2871 LE
int incomingByte;

/* Move this into a struct
   All the values required by the spi_write() command */
uint8_t spi_select; // Currently selected LO (1, 2 or 3) that is to be programmed
uint32_t spiWord;   // Holds the register contents to be written to the selected device

MAX2871_LO LO = MAX2871_LO();

// When the mux pin is configured for Digital Lock Detect output
// we can read the status of the pin from here.
void MuxTest(String chipName) {
  if (digitalRead(PLL_MUX)) {
    Serial.print(chipName);
    Serial.println(" LOCK   ");
  }
  else {
    Serial.print(chipName);
    Serial.println(" UNLOCK   ");
  }
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
    z = LO.Curr.Reg[x];                      // Program remaining registers where LO=3915 MHz
    spiWriteLO(z, selectPin);                // and Lock Detect is enabled on the Mux pin
  }
  delay(1);                                  // Short delay before reading Register 6
  MuxTest("LO");                             // Check if LO is locked by reading the Mux pin
  spiWriteLO(LO.Curr.Reg[6], selectPin);     // Tri-stating the mux output disables LO2 lock detect
}

// Program a single register of the selected LO by sending and latching 4 bytes
void spiWriteLO(uint32_t reg, uint8_t selectPin) {
  digitalWrite(MAX2871_SEL, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, reg >> 24);
  shiftOut(dataPin, clockPin, MSBFIRST, reg >> 16);
  shiftOut(dataPin, clockPin, MSBFIRST, reg >> 8);
  shiftOut(dataPin, clockPin, MSBFIRST, reg);
  delayMicroseconds(10);
  digitalWrite(MAX2871_SEL, HIGH);
  delayMicroseconds(10);
  digitalWrite(MAX2871_SEL, LOW);
}

//Test lockPin.RF_en_swx, drive GRN light
void Status() {
  int stat = 0;

  if (digitalRead(RF_en_swx) == HIGH ) {
    stat = 1;
    digitalWrite(RF_En, HIGH);
  } else {
    digitalWrite(RF_En, LOW);
  }

  if (digitalRead(lockPin) == HIGH ) {
    stat = stat + 2;
  }
  if (stat == 3 ) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  // stat=0 unlock,RF off; 1= unlock,RF on; 2=lock, RF off; 3=lock,RF on
}


int addr;

void setup() {
  // set pins to output because they are addressed in the main loop
  pinMode(lockPin, INPUT);
  pinMode(RF_En, OUTPUT);
  pinMode(strobe, OUTPUT);
  pinMode(RF_en_swx, INPUT_PULLUP);
  pinMode(PLL_MUX, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(MAX2871_SEL, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(2000000);
  digitalWrite(RF_En, LOW);     // Initially turn off MAX2871 RF Output
  digitalWrite(LED_BUILTIN, HIGH);       // Initially, turn off Lock Detect Indicator
  digitalWrite(MAX2871_SEL, LOW);  // Latch must start LOW
  digitalWrite(strobe, HIGH);    // Strobe HIGH so you know it's ready before starting
//  Serial.println("MAX2871_Load_Word_115200_5.ino 21 May 2021");

  initialize_LO(MAX2871_SEL, true);
  initialize_LO(MAX2871_SEL, false);
}

void loop() {
  if (Serial.available()) {
    z = Serial.parseInt();
  }
  
  if (z > 0) {
    addr = z & 0x00000007;    // First 3 bits are the chip Register address
    if (LO.Curr.Reg[addr] != z) {
      Serial.println("Reg[addr] != z");
      LO.Curr.Reg[addr] = z;
      spiWriteLO(LO.Curr.Reg[addr], MAX2871_SEL);
    }
    Status();
  }
  z = 0;
}
