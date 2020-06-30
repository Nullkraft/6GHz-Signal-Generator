PROJECT#3 MAX2871 Synthesizer Tests 27June2020 WN2A

1) List of files in this folder:

   A) Arduino UNO R3 Code:  MAX2871_Load_Word_115200_3.ino
                    All tests reported here done with this program.
                      
   B) FB Host Code: Used to Command a single frequency manually or sweep.
                   MAX2871_Command_4 (Linux Executable)
                   MAX2871_Command_4.bas (FB source code)
                   
   C) Screenshots of MAX2871_Command_4 running:                 
                     MAX2871_Sweep_1200-1500_401pts_1ms_DefaultFrac.png
                     MAX2871_Sweep_1200-1500_401pts_1ms_FracOpt-Beta.png
                                        
                   
   D) Oscilloscope traces of PROJECT#3 running with  A) and B) showing PLL Lock time
                  Timing_MAX2871_Sweep_1200-1500MHz_401pts_1ms_DefaultFrac.jpg
                  Timing_MAX2871_Sweep_1200-1500MHz_401pts_1ms_FracOpt.jpg
                  
   E) MAX2871_1200-1500MHz_401pts_FractOpt.mpg. 
                  Video of the sweep mode. Sweeping 1200-1500MHz
                  401 points. This is the Fraction Optimized mode, which is experimental.
                  The Default Fraction mode is faster, due to fewer registers being updated
                  for the default fraction mode.
                  
2) Notes:
    A) Huge improvement!!! Now being able to sweep at 900 points/second (default Fraction mode)
      will permit 201 point sweeps made at>4 sweeps/sec and 401 point sweeps at >2 sweeps/sec.
      This makes the MAX2871  (LO#2) not the ADF4356 (LO#1) the preferred device to make the
      most frequent frequency changes, as the MAX2871 is faster. 
      
    B) The default fraction mode clocks in at 1.11 millisec/point (900 points/sec).
       Normally this only requires one register update from point to point, except when
       band changing. The observed spectrum looks clean as seen on my old HP8558B 
         
    C) The Fraction Optimized mode clocks in at ~2 millisec/point (500 points/sec).
       This mode normally requires two register updates from point to point,
       sometimes only one, and only when there is a band change it may require two or three.
       Note,the timing waveform shows how the cycles will vary.
       This mode should improve the Fractional-N spurs, but I cannot verify that on
       my old HP8558B.
        
    D) As before ,the little "Arduino quirk" is still there:
        In order for the Host Program to get control of the Arduino,
         this procedure was needed:
         a) Connect Arduino to Laptop via USB cable
         b) Open the Arduino IDE, check the Serial Port # (0,1,2,etc)
         c) Open the Serial Monitor. Now close the Serial Monitor and IDE.
         d) Run the FB program, either one B) or C) above.
         
         You didn't need to load any sketches or enter anything into the Serial Monitor,
         but it seemed weird that you need to open the IDE AND the Serial Monitor.
         Weird?
                                       
                  
                  
                                            
          
                