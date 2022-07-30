'MAX2871_Command_10.bas for Signal Generator / Spectrum Analyzer Freq Synthesis.

Declare sub Math
Declare sub Transmit
Declare sub TransmitF

dim shared as double Fvco, N, FracT
dim shared as ulong NI, registerNum, Reg(6,401), OldReg(6) ' Reg(6, 401) is Reg(registerNum, nSteps). 6 registers;401 max frequency steps
dim shared as string Sweep, freq, ComStr, portNum, portSpeed, delaystr, FracOpt,Message,LD,portType,RFport,Ans
dim shared as single Fpfd, Fref, RFOut, FrfEst, FvcoEst, FracErr, Finc, Frfv(401),Initial
dim shared as integer Range, OldRange, delay, RefDiv, nSteps '
dim shared as integer MOD1, Fracc, NewMOD1, NewFracc, Mode,Freqs,Div
dim shared as single Fstart,Fstop
dim shared as integer x,y,z,w,h,RFportBits,SweepNum




const as integer initialized = 1    ' MAX2871 not yet initialized
dim shared as integer MAX2871

MAX2871 = not initialized

' Added for readability - Mark
dim shared as integer serPort = 1

' Begin execution of our program
Program_Start:
  screeninfo w,h :if h<600 then screen 18 else screen 19 
  cls ' cls isn't really used at this point(that true!)
  color 11,0

  print
  print " MAX2871_Command_10.bas by WN2A/MS for PROJECT#3 Signal Generator"
  print " Use with Arduino MAX2871_Load_Word_115200_ 5.ino"
  print " Supports 23.5-6000MHz. REF freq=60MHz, Fraction_Optimization"
  print "  1) With above sketch loaded into Arduino, connect Arduino UNO,"
  print "      and start the Arduino IDE"
  print "  2) Check for Serial Port#, open Serial Monitor, set Baud=115200. Close IDE"
  print " Refer to README_MAX2871_Command.bas.txt ":print
  input " Now enter ttyACM or ttyUSB Port 0,1,2, etc < 0 default> "; portNum : if portNum="" then portNum="0"
  input " If ttyUSB enter 'U' and enter, if just ACM, just press enter"; portType : 

  ' Opening the serial port...
  portSpeed="115200"
  if portType="" then ComStr = "/dev/ttyACM" + portNum + ":" + portSpeed + ",N,8,1"      ' NOT a colon separated command
  if portType=LCase("U") then ComStr = "/dev/ttyUSB" + portNum + ":" + portSpeed + ",N,8,1"      ' NOT a colon separated command
  print str(ComStr)
  open com ComStr as #serPort

'  input " Enter delay < 2 millseconds >"; delaystr : if delaystr="" then delaystr="2"
'  delay = val(delaystr)
'  MessageLoop: ' An optional section telling you the Arduino is communicating with the PC
'  if inkey="q" then goto exitProg 
'  input #serPort, Message
'  if Message="" then goto MessageLoop
'  print " Arduino Message: ";Message
  open "codes.txt" For Output As #42

  sleep 500

  input " Enter Fref in MHz (just press Enter for 60MHz) ";Fref ' Reference clock frequency 60 or 100 MHz
  if Fref=0 then Fref=60
  input " Enter Reference Divide Ratio 1-1023  <Default=2>"; RefDiv :if RefDiv=0 then RefDiv=2
  Fpfd=(Fref*1e6)/RefDiv :print " Fpfd= ";Fpfd/(1e6);" [MHz]"
  
  input " <n> disables Lock Detect on Mux Line, <y> is default ";LD :if LD="" then LD="y"
  
  input " Use Fraction Optimization <Enter> or Fix MOD1=4095 <m> "; FracOpt :?
  RFportBits=0
  input "Do you want RF Output A enabled? <y or enter>";RFport : if RFport="y" then RFportBits=(2^5)
  input "Do you want RF Output B enabled? <y or enter>";RFport : if RFport="y" then RFportBits=(2^8)+RFportBits
        
  input " Swept <s> or Default Fixed Frequency Mode<Enter>? "; Sweep
  if Sweep="s" then goto Swept


Manual:
  print	' Prints an empty line
  input " Enter Frequency [MHz] , <r> to restart or <q> to quit"; freq
  if freq="q" then goto exitProg
  if freq="r" then goto Program_Start
  RFOut=val(freq)	 ' String to float
  if (23.5<=RFOut) and (RFOut<=6000) then goto OkayValue
  Print " Sorry Dave, I'm afraid I can't do that."	' Warn user: Requested frequency exceeds range of MAX2871
  goto Manual

OkayValue:
  Math 'Do The Math first so FracT is available to the Transmit/F functions.
  if MAX2871 = initialized then TransmitF else Transmit
  goto Manual

Swept:
  input " Enter Start Frequency [MHz] "; Fstart
  input " Enter Stop Frequency [MHz] "; Fstop
  input " Enter Number of Frequencies <2-401> "; Freqs
  for nSteps=0 to (Freqs-1)
	Frfv(nSteps) = Fstart + nSteps * (Fstop - Fstart)/(Freqs - 1)
    RFOut = Frfv(nSteps)
    Math 'Do The Math first: nSteps, Frfv(nSteps) >> Reg(registerNum, nSteps)
  next
  SweepNum=0
  while inkey <> "q"
  SweepNum=SweepNum+1
    for nSteps = 0 to (Freqs - 1)
      if MAX2871 = initialized then TransmitF else Transmit
     cls :  print " Now Sweep # ";SweepNum;"  Press and hold <q> to quit sweeping"
     next
  wend
  goto exitProg

''''''''''''''''''''''''''''''''''''''''
Datarow1: 'For MAX2871 Initialization, supports LD on Mux Line Reg0>>Reg 5
  data  1737040, 1073808361, 2550161218,     7971, 1675526652,  4194309 '100.000 MHz initial frequency
      '001A8150,   400103E9,   98005F42, 00001F23,   63DE81FC, 00400005  
Datarow2: 'For MAX2871 Initialization, without LD on Mux Line Reg0>>Reg 5
  data  1737040, 1073808361, 2550161218,     7971, 1676575228,  4194309 '50.000 MHz no LD initial frequency
      '001A8150,   400103E9,   98005F42, 00001F23,   63EE81FC, 00400005 

exitProg:
  print "Goodbye,Adios,Aloha,73!"
  close #serPort
  sleep 2000
  end

sub Transmit 'This routine initializes MAX2871 with nSteps=0

  if Sweep<>"s" then open "newRegisters.txt" for output as #42 'Douglas Adams was here
  print " First we initialize the MAX2871 (twice)"
  for z=0 to 1 'do the Reg 5-4-3-2-1-0 cycle twice
    for y = 0 to 5 : registerNum = (5 - y)
      write #serPort, Reg(registerNum, 0)
     if Sweep<>"s" then print #42, hex(Reg(registerNum,0)) 'and here too
     if Sweep<>"s" then print " Transmit: Reg("; registerNum; ",0) = ";hex(Reg(registerNum,0));" NI=";NI;" Fracc=";Fracc;" MOD1=";MOD1
      sleep 100
      OldReg(registerNum) = Reg(registerNum, 0)
    next y

  next z
 
  MAX2871 = initialized
  if Sweep<>"s" then close #42 ' Goodbye
end sub


sub TransmitF 'For changes in Frequency sweep or Power levels. Enter with Ref(registerNum,nSteps), nSteps
  for y = 0 to 5
    registerNum = (5 - y)
    if (Reg(registerNum, nSteps) = OldReg(registerNum)) then goto SkipOver
      write #serPort, Reg(registerNum, nSteps)
      sleep(delay)
      OldReg(registerNum) = Reg(registerNum, nSteps)
    if Sweep<>"s" then print " TransmitF: Reg("; registerNum; ",0) = "; hex(Reg(registerNum,0)); " NI="; NI; " Fracc="; Fracc; " MOD1="; MOD1
    input #serPort, Message 
    print Message 'READ BACK STATUS VALUE
SkipOver:
  next
end sub


sub Fraction_Opt 'Enter with FracT, leave with Fracc , MOD1
  Initial = 2
  'print "From Fraction_Opt Fract = ";Fract
  for MOD1 = 4095 to 2 step -1
    Fracc = int(FracT * MOD1)
    FracErr = abs(FracT - (Fracc/MOD1))
    if (FracErr > Initial) then goto Jumpover
      NewFracc = Fracc
      Initial = FracErr
      NewMOD1 = MOD1
    Jumpover:
	next
  MOD1 = NewMOD1
  Fracc = NewFracc
end sub

' Math is called once for each step in the frequency sweep function
sub math   'Enter with Fpfd, RFOut, registerNum >> Leave with Reg(registerNum, nSteps)
  for registerNum = 0 to 7  'Determine divider range
    Div=(2^registerNum)
    if (RFOut * Div) >= 3000 then goto Main
  next
Main:  'Divider range now known
  Range = registerNum
  Fvco = Div * RFOut
  Print "  Fvco= ";Fvco;" [MHz] Divider=";Div;" RFOut ="; RFOut;" [MHz] "
  N = 1e6 * (Fvco/(Fpfd))
  NI = int(N)
  FracT = N - NI
  if FracOpt<>"m" then Fraction_Opt : goto JumpMod
    MOD1 = 4095
    Fracc = int(FracT * MOD1)
JumpMod:
  FvcoEst = Fpfd * (1e-6) * (NI + (Fracc/MOD1))
  FrfEst = FvcoEst/(Div)

  'restore Datarow1 'Restore Data Pointer 1 unless...
  if LD="y" then restore Datarow1 else restore Datarow2

  for registerNum = 0 to 5 : read Reg(registerNum, nSteps) : next 'Initialize Reg(registerNum, nSteps) default decimal Values

  ' These DON'T use registerNum but are hard coded as register number N. 
  ' They replace values from the fixed values for  Register 0,1,2,4
  Reg(0,nSteps) = (NI * (2^15)) + (Fracc * (2^3)) ' For N Register + Frac Register+0
  Reg(1,nSteps) = (2^29) + (2^15) + (MOD1*(2^3)) + 1 ' CP Lin,MOD1+1
  
  Reg(2,nSteps) = (2^30)+(RefDiv*(2^14))+(2^12)+(2^9)+(2^6)+2 'Low Spur1,R Div,3.2mA,Pol,2
  if Fpfd>(32e6) then Reg(2,nSteps)=Reg(2,nSteps)+(2^31)  'If Pfd>32MHz then Lock Speed Detect
  if LD="y" then Reg(2,nSteps)=Reg(2,nSteps)+((2^28)+(2^27)) ' Add for LD only
  
  '  Reg(4,nSteps) = 1670377980 + ((2^20) * Range)
     Reg(4,nSteps) = 1670377692 + ((2^20) * Range)+RFportBits

end sub














