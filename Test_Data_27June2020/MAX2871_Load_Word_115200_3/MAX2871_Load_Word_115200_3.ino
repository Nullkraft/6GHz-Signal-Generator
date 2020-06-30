//************************************************
//  Name    : MAX2871_Load_Word_3.ino                 
//  Author  : WN2A and Mark Stanley                                
//  Date    : 27 June 2020                         
//  Version : 1.0                                
//  Notes   : Code for using MAX2871 Synth (1 Register at a time)
//  Use with FreeBasic Program MAX2871_Command_4.bas 
//************************************************

unsigned long q,z;
int (x);
int latchPin = 17;  //LE
int clockPin = 15;  //SCLK
int dataPin = 16;   //DATA
int strobe = 10;  //STROBE
int incomingByte;

void setup() {
    //set pins to output because they are addressed in the main loop
  pinMode(5,OUTPUT); //RF_En//
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(strobe, OUTPUT);  
  pinMode(13,OUTPUT);  
  Serial.begin(115200);
  q = z= 0;
  Serial.println("MAX2871_Load_Word_115200_3.ino 27 June 2020");
  digitalWrite(5, HIGH); //This Turns on the RF Output//
  digitalWrite(13,HIGH); //LED ON  
}
  
void loop() {  
  if (Serial.available()) {   
       z = Serial.parseInt(); //This is much better!
}
if (z>0) {
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
