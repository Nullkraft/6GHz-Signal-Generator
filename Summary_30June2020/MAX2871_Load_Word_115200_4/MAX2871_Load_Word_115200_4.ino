//************************************************************************************************
//  Name    : MAX2871_Load_Word_4.ino
//  Author  : WN2A and Mark Stanley
//  Date    : 27 June 2020
//  Version : 1.0
//  Notes   : While working on the design of a 3 GHz Spectrum Analyzer, Newark Electronics recommended
//          : the MAX2871 23.5MHz to 6000MHz Fractional/Integer-N Synthesizer/VCO as a replacement for
//          : the ADF4355. The Maxim chip was only $11 USD compared to $44 for the Analog Devices 
//          : component. But the ADF4355 had already been incorporated, and the software written, for 
//          : the current Synthesizer design. What we needed was a simple demo board that could be used
//          : to test some code for the chip. Maxim sells exactly this but we wanted to be able to 
//          : release it as open source and open hardware. You can find what you need to build it at:
//          : 
//          :     https://github.com/Nullkraft/6GHz-Signal-Generator
//          : 
//          : This sketch, MAX2871_Load_Word_4.ino, was written as a minimal hardware driver to test
//          : the operation of the MAX2871. It requires the use of a companion program 
//          : (FreeBasic: MAX2871_Command_4.bas) running on the host PC. The host PC calculates the 
//          : register values from the frequency given by the user.
//************************************************************************************************

unsigned long z;
int latchPin = A3;  // MAX2871 LE
int clockPin = A1;  // MAX2871 SCLK
int dataPin = A2;   // MAX2871 DATA
int strobe = 10;    // MAX2871 STROBE
int RF_En = 5;      // MAX2871 RF_EN
int LED = 13;       // Arduino LED
int incomingByte;

int count;

void setup() {
  Serial.begin(115200);
  Serial.println("MAX2871_Load_Word_115200_4.ino 2 July 2020");
  
  // set pins to output because they are addressed in the main loop
  pinMode(RF_En, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(strobe, OUTPUT);
  pinMode(LED, OUTPUT);
  
  digitalWrite(RF_En, HIGH);      // Turn on the MAX2871 RF Output
  digitalWrite(LED, LOW);        // Turn on the Arduino LED
  digitalWrite(latchPin, HIGH);    // Latch must start LOW
  digitalWrite(strobe, LOW);      // Strobe must start LOW
  z = 0;
  count = 0;
}

void loop() {
  if (Serial.available()) {
    z = Serial.parseInt();
  }

  if (z > 0) {
    setFrequency();
  }
}

// Load 4 control registers of the MAX2871 for setting the frequency
void setFrequency() {
//  digitalWrite (strobe, HIGH);
//  delayMicroseconds(10);
//  digitalWrite (strobe, LOW);
  digitalWrite(latchPin, LOW);
  delayMicroseconds(10);
  shiftOut(dataPin, clockPin, MSBFIRST, z >> 24);
  shiftOut(dataPin, clockPin, MSBFIRST, z >> 16);
  shiftOut(dataPin, clockPin, MSBFIRST, z >> 8);
  shiftOut(dataPin, clockPin, MSBFIRST, z);
  delayMicroseconds(10);
  digitalWrite(latchPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(latchPin, LOW);
}



/* Although you can download this from github I provide here just for convenience.
   MAX2871_Command_4.bas Source Code

 ******************* COPY BELOW THIS LINE *********************************


 'MAX2871_Command_4.bas for Spectrum Analyzer Freq Synthesis.
dim shared as ulong NI,x,y,z,r,Fract1,Fract2,MOD2,Reg(6,401),FpfdH,FchspH,OldReg(6),slow
 ' Reg(6,401) is Reg(x,f). 6 registers;401 max frequencies
dim shared as double Fvco,N,FracT,Num,Fchsp,Re,Fout,TimeA,TimeB
dim shared as string Ans,ComStr,Ans1,Ans2,t,delaystr,FracOpt
dim shared as string delayst,Message,Remark,Sweep
dim shared as integer div,Flag,Range,OldRange,delay,RefDiv,f
dim shared as integer MOD1,Fracc,NewMOD1,NewFracc,Mode
dim shared as single Fstart,Fstop,Freqs,Fpfd,Fref,Frf,FrfEst,FvcoEst,Initial,FracErr,Finc,Frfv(401)
Flag=0 : Remark=" Sorry Dave, I'm afraid can't do that "

sub Transmit 'This routine initializes MAX2871 at f=0
print "First we initialize the MAX2871 (twice)"
for z=0 to 1 'do the Reg 5-4-3-2-1-0 cycle twice
for y=0 to 5 : x=(5-y) 
  write #1,Reg(x,0) 'print "Hello from Transmit x= ";x;" Reg(x,0)= ";Reg(x,0);" NI=";NI;" Fracc=";Fracc;" MOD1=";MOD1
  sleep 100
  OldReg(x)=Reg(x,0)
next y
next z
Flag=1 
end sub

sub TransmitF 'For Frequency/Range or Power changes. Enter with Ref(x,f), f
 for y=0 to 5 :x=(5-y)
 if (Reg(x,f)<>OldReg(x)) then  write #1,Reg(x,f) :sleep(delay) ':Print x,f,Reg(x,f) 'Only write as required   
  OldReg(x)=Reg(x,f)
  next
end sub

sub Fraction_Opt 'Enter with FracT, leave with Fracc , MOD1
Initial=2 ': print "From Fraction_Opt Fract= ";Fract
for MOD1=4095 to 2 step -1
Fracc=int(FracT*MOD1) : FracErr=abs(FracT-(Fracc/MOD1))
if (FracErr>Initial) then goto Jumpover
NewFracc=Fracc: Initial=FracErr : NewMOD1=MOD1
Jumpover:
next
MOD1=NewMOD1 :Fracc=NewFracc
end sub

sub math   'Enter with Fpfd,Frf,f. >> Leave with Reg(x,f) 
 for x=0 to 6  'Determine divider range
  Div=(2^x)
  if (Frf*Div)>=3000 then goto Main
 next
Main:  'Divider range now known
Range=x : Fvco=Div*Frf
'Print "  Fvco= ";Fvco;" [MHz] Divider Ratio=";Div;" Frf= ";Frf;" [MHz]  Range= ";Range
N=1e6*(Fvco/(Fpfd)) : NI=int(N) : FracT=N-NI : 
if FracOpt="f" then Fraction_Opt : goto JumpMod  'Fraction_Opt : goto JumpMod
MOD1=4095 : Fracc=int(FracT*MOD1)
JumpMod: 

FvcoEst=Fpfd*(1e-6)*(NI+(Fracc/MOD1)) : FrfEst=FvcoEst/(Div) 
'print "N= ";N;" NI= ";NI;" Fracc= ";Fracc;" MOD1= ";MOD1;" Fpfd= ";Fpfd
'print "FcvoEst= ";FvcoEst;"[MHz]    FrfEst= ";FrfEst;"[MHz]   FerrEst= ";int((1e6)*(FrfEst-Frf));"[Hz]"

restore Datarow1 'Restore Data Pointer 1
for x=0 to 5 : read Reg(x,f) : next 'Initialize Reg(x,f) default decimal Values
Reg(0,f)=(NI*(2^15))+(Fracc*(2^3))
Reg(1,f)=(2^29)+(2^15)+(MOD1*(2^3))+1
Reg(4,f)=1670377980+((2^20)*Range)
end sub

Setup:
screen 18 :cls
print "MAX2871_Command_4.bas by WN2A for PROJECT#3 Signal Generator"
print "Use with Arduino MAX2871_Load_Word_115200_3.ino"
print "Supports 23.5-6000MHz. REF freq=60MHz, Experimental version with Fraction_Opt"
print "********   This version is only Beta!  ********************"
print " 1) With above sketch loaded into Arduino, start the Arduino IDE"
print " 2) Check for Serial Port#, open Serial Monitor, and set Baud=115200. Close IDE" 
print "Refer to README_MAX2871_Command.bas.txt !!":print
input "Now enter ttyACM Port 0,1 or 2 < 0 default> ";Ans1 : if Ans1="" then Ans1="0"
Ans2="115200" : Fref=60e6 : RefDiv=4 : Fpfd=Fref/RefDiv
ComStr="/dev/ttyACM"+Ans1+":"+Ans2+",N,8,1"
open com Comstr as #1

 
input "Enter delay < 2 millseconds >";delaystr : if delaystr="" then delaystr="2"
input #1 ,Message : print Message  :print
delay=val(delaystr)
input "Enable Experimental Fraction Optimization <f> or Default <Return> "; FracOpt
input "Swept <s> or Default Fixed Frequency Mode<Return>? ";Sweep :if Sweep="s" then goto Swept

Manual:
print
input "Enter Frequency [MHz] <q> to quit";Ans : if Ans="q" then goto Sleepy
Frf=val(Ans) : if (23.5>Frf) or (Frf>6000) then print Remark 
Math
if Flag=0 then Transmit else TransmitF
goto Manual

Swept:
 input "Enter Start Frequency [MHz] ";Fstart
 input "Enter Stop Frequency [MHz] ";Fstop
 input "Enter Number of Frequencies <2-401> ";Freqs
 for f=0 to (Freqs-1) :Frfv(f)=Fstart+f*(Fstop-Fstart)/(Freqs-1):Frf=Frfv(f)
 Math 'Do The Math First: f,Frfv(f)>> Reg(x,f) 
 next
 print "Math done! Press and hold <q> to quit sweeping"
WrapBack:
 for f=0 to (Freqs-1)
 if Flag=0 then Transmit else TransmitF
 next
 if inkey$<>"q" then goto Wrapback 
Sleepy:
close #1
sleep

Datarow1: 'For MAX2871 Initialization
  data  13093080,541097977,1476465218,4160782339,1674572284,2151677957 '374.596154 MHz

  
  
 ******************* COPY ABOVE THIS LINE *********************************
 
 */
