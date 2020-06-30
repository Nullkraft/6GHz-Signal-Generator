README for PROJECT#3 Synthesizer 6 April 2020- WN2A

A synthesizer compatible with Arduino UNO R3 Microcontroller and others.

1) Schematics: are Unembedded: wn2a-SigGen-page1B_Unembed.sch
                   Enbedded: wn2a-SigGen-page1B_Embed.sch
    
   A combination of Mark Stanley/Mike Masterson gEDA .schematics 
    
2) BOM is PROJECT#3_Synthesizer.ods

3) MAX2871 is drawn on schematic as it looks from its top-view.
    The reason is so to better visualize the optimal positions of bypass capacitors,
    for the purpose of maximizing output power especially above 3000 MHz. 

3) Design is intended to meet preliminary specifications:
   Frequency Range :23.5-6000 MHz. Step size 1KHz at min setting (TBD)
   RF Power approx -5 to +4dBm
   Harmonic Output ~ -30dBc
   Dual outputs, with divider options per MAX2871
   Powered by either by Arduino UNO R3 OR External +7 ~ +13V (Schottky Diode OR'd)
   Programmed with same interface and sketches as PROJECT#1 LO#2 ,
    (For which the MAX2871 will be LO#2 and LO#3)
   XTAL Reference 60.000 MHz for low phase noise.
   Individual AZ1117 low-noise regulators for Digital,RF and VCO
   Broadband output network.
   LOCK DET SMD LED indicator 
   Compatible with Arduino UNO R3.