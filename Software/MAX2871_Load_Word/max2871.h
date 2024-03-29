#ifndef _MAX2871_
#define _MAX2871_

#include <Arduino.h>    // Includes typedef for uint32_t


/* Default register values for MAX2871 LO: Sets default RFOout = 80 MHz */
typedef struct maxRegisters {
  static const byte numRegisters = 6;
  static const byte numProgrammableRegs = 5;
  uint32_t Reg[numRegisters] = { 
                                 0x002A8150,  //    2785616
                                 0x400103E9,  // 1073808361
                                 0x98005F42,  // 2550161218   Digital Lock detect ON
                                 0x00001F23,  //       7971
                                 0x63EFF1FC,  // 1676669436   RFout_A disabled
                                 0x00400005,  //    4194309   Bit 18 is MSB for Digital Lock detect control
                               };
//  uint16_t* RLO2_as_int = (uint16_t*)RLO2;
} max2871Registers;


class MAX2871_LO {
  public:
    void begin(float initial_frequency);

    const max2871Registers Default;   // Default read-only copy of the registers
    max2871Registers Curr;            // Modifiable copy of the registers for LO3

    /* 6 bit mask of Embedded Data from serial Specific Command */
    const short Data_Mask = 0x3F;

    /* 12 bit mask, R[1] bits [14:3], for Fractional Modulus Value, M */
    const uint32_t M_mask = 0x7FF8;

    /* 12 bit mask, R[0] bits [14:3], for Frequency Division Value, F */
    const uint32_t F_mask = 0x7FF8;

    /* 8 bit mask, R[0] bits [22:15], for Integer Counter, N */
    const uint32_t N_mask = 0x7F8000;

    /* 20 bit mask, R[0] bits [22:3], for N and F */
    const uint32_t NF_mask = 0x7FFFF8;

    /* R4<8> and R4<5> disable RFoutB and RFoutA */
    const uint32_t RFpower_off = 0xFFFFFE07;

    /* R4<7:6> Adjust the RFoutB power level. (RFoutA is off by default) */
    /* Also provides a differenct name to make the main sketch more readable */
    const uint32_t Power_Level_Mask = RFpower_off;

    /* R4<7:6> Adjust the RFoutB power level. (RFoutA is off by default) */
    /* The MSbit, R4<8>, ensures that the RFout is enabled */
    const uint32_t neg4dBm = 0x100;
    const uint32_t neg1dBm = 0x140;
    const uint32_t pos2dBm = 0x180;
    const uint32_t pos5dBm = 0x1C0;

    /*****  ----------------------- NOTE: --------------------------  *****/
    /***** | Enabling Tristate, Mux_Set_TRI, automatically disables | *****/
    /***** | Digital Lock Detect, Mux_Set_DLD, and vice versa.      | *****/
    /*****  --------------------------------------------------------  *****/

    /* 'AND' Mux_Set_TRI with R2 to enable Tristate. Affects bits <28:26> */
    const uint32_t Mux_Set_TRI = 0xE3FFFFFF;

    /* 'OR' Mux_Set_DLD with R2 to enable Digital Lock Detect. Affects bits <28:26> */
    const uint32_t Mux_Set_DLD = 0x18000000;

    uint32_t spiMaxSpeed = 20000000;   // 20 MHz max SPI clock
};


#endif



// Do LO power level commands next
