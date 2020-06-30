//************************************************
//  Name    : MAX2871_Load_Word_1.ino                 
//  Author  : WN2A                                
//  Date    : 19 June 2020                         
//  Version : 1.0                                
//  Notes   : Code for using MAX2871 Synth (1 Register at a time)
//  Use with FreeBasic Program MAX2871_TestCom_1.bas 
//************************************************

unsigned long z;
unsigned long q;
int (x);
String readString;
int latchPin = 17;  //LE
int clockPin = 15;  //SCLK
int dataPin = 16;   //DATA
int strobe = 10;  //STROBE

void setup() {
    //set pins to output because they are addressed in the main loop
  pinMode(5,OUTPUT); //RF_En//
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(strobe, OUTPUT);  
  Serial.begin(115200);
 // Serial.println("MAX2871_Load_Word_1.ino  19 June 2020 Takes U32 word from USB to SPI Bus ShiftOut");
//  Serial.println("For use with the PROJECT#3 Synthesizer @ 115200 Baud");
  digitalWrite(5, HIGH); //This Turns on the RF Output//
  delay(1);
}
void loop() {
  
  while (Serial.available()) {
    delayMicroseconds(1000);
   char c = Serial.read();
   readString += c;
 }
 if (readString.length() >0) {
   z=readString.toInt();
      readString=""; 
      q=z/65536;
  delayMicroseconds(10);
     digitalWrite (strobe, LOW);
     digitalWrite (strobe, HIGH);
       delayMicroseconds(10);
         digitalWrite (strobe, LOW);
 Shifty(x);
 }
}

 void Shifty(int x) {  //Send out (4) 8-Bit Bytes then Latch 
     digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST,(q>>8)); 
     shiftOut(dataPin, clockPin, MSBFIRST,q); 
     shiftOut(dataPin, clockPin, MSBFIRST,(z>>8));
     shiftOut(dataPin, clockPin, MSBFIRST,z);
    delayMicroseconds(1); 
    digitalWrite(latchPin, HIGH);
    delayMicroseconds(10);
        digitalWrite(latchPin, LOW);
  }
