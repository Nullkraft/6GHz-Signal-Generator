'MAX2871_Command_4.bas for Spectrum Analyzer Freq Synthesis.
dim shared as ulong NI,x,y,z,r,Fract1,Fract2,MOD2,Reg(6,401),FpfdH,FchspH,OldReg(6),slow
 ' Reg(6,401) is Reg(x,f). 6 registers;401 max frequencies
dim shared as double Fvco,N,FracT,Num,Fchsp,Re,Fout,TimeA,TimeB
dim shared as string Ans,ComStr,Ans1,Ans2,t,delaystr,FracOpt
dim shared as string delayst,Message,Remark,Sweep
dim shared as integer div,Flag,Range,OldRange,delay,RefDiv,f
dim shared as integer MOD1,Fracc,NewMOD1,NewFracc,Mode, idx
dim shared as single Fstart,Fstop,Freqs,Fpfd,Fref,Frf,FrfEst,FvcoEst,Initial,FracErr,Finc,Frfv(401)
Flag=0 : Remark=" Sorry Dave, I'm afraid can't do that "

sub Transmit 'This routine initializes MAX2871 at f=0
print "First we initialize the MAX2871 (twice)"
for z=0 to 1 'do the Reg 5-4-3-2-1-0 cycle twice
    for y=0 to 5 : x=(5-y) 
      write #1,Reg(x,0) 'print "Hello from Transmit x= ";x;" Reg(x,0)= ";Reg(x,0);" NI=";NI;" Fracc=";Fracc;" MOD1=";MOD1
      if Flag = 1 then print Reg(x, 0)
      sleep 100
      OldReg(x)=Reg(x,0)
    next y
    Flag=1
next z
Flag=1
end sub

'sub TransmitF 'For Frequency/Range or Power changes. Enter with Ref(x,f), f
    'idx = 0
    'write #1, Reg(idx, f)
    'print idx, "Transmit"
'end sub

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
'Reg(4,f)=1670377764+((2^20)*Range)     ' -4 dB RF Output power
'Reg(4,f)=1670377908+((2^20)*Range)      ' +2 dB RF Output power
Reg(4,f)=1670377980+((2^20)*Range)      ' +5 dB RF Output power
end sub

Setup:
'screen 18 :cls
print "MAX2871_Command_4.bas by WN2A for PROJECT#3 Signal Generator"
print "Use with Arduino MAX2871_Load_Word_115200_3.ino"
print "Supports 23.5-6000MHz. REF freq=60MHz, Experimental version with Fraction_Opt"
print "********   This version is only Beta!  ********************"
print " 1) With above sketch loaded into Arduino, start the Arduino IDE"
print " 2) Check for Serial Port#, open Serial Monitor, and set Baud=115200. Close IDE" 
print "Refer to README_MAX2871_Command.bas.txt !!":print
'input "Now enter ttyACM Port 0,1 or 2 < 0 default> ";Ans1 : if Ans1="" then Ans1="0"
input "Now enter ttyUSB Port 0,1 or 2 < 1 default> ";Ans1 : if Ans1="" then Ans1="1"
Ans2="115200" : Fref=60e6 : RefDiv=4 : Fpfd=Fref/RefDiv
ComStr="/dev/ttyUSB"+Ans1+":"+Ans2+",N,8,1"
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

'for m=0 to 9
    if Flag=0 then Transmit else TransmitF
'next m

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
'  data  13093080, 541097977, 1476465218, 4160782339, 1674572068, 2151677957 '374.596154 MHz
'      different  different    !Read R6!     same       -4 dBm       Wrong                   
'  data   6990504, 536936441, 1073779266, 4160782339, 1675620860,   20971525 '374.596154 MHz
'          N      phase val     same        same       +5 dBm                                 
  data   4292616, 536903697, 1073779266, 4160782339, 1674572228, 20971525 '374.596154 MHz

