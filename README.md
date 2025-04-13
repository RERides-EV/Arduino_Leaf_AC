# Arduino_Leaf_AC
Control a Leaf AC compressor with Arduino

arduino_leaf_ac.h and arduino_leaf_ac.cpp files should be placed in the Arduino project folder.

arduino_leaf_ac.h should be included in the .ino file of your build

    #include "arduino_leaf_ac.h"

I have not tested yet, but the following control data is from others who have had success. It may need to be modified based on year and/or manufacturer of the compressor 

    // Private variables
    static int brklen = 14;      // 13 bits + 1 bit for break delimiter
    static uint8_t sync = 0x55;
    static uint8_t addr = 0xfb;
    static uint8_t currentPowerLevel = DEFAULT_AC_KW;
    static uint8_t buf[8] = {AC_OFF_CMD, AC_ZERO_KW, 0x00, 0x90, 0xFF, 0x00, 0x00, 0x00};  
    static uint8_t cksum = 0xb7;
    static int baud = 19200;
    static uint16_t brk = (brklen*10000/baud); // brk in microseconds

    // AC power settings for future implementation
    #define AC_ZERO_KW 0x00       // 0kW (OFF)
    #define AC_POWER_LOW 0x05     // 1kW
    #define DEFAULT_AC_KW 0x0c    // ~1.5kW
    #define AC_POWER_MED 0x12     // 2kW
    #define AC_POWER_HIGH 0x16    // 3kW

    // AC command bytes
    #define AC_ON_CMD 0xb3        // Command byte for AC ON
    #define AC_OFF_CMD 0xb2       // Command byte for AC OFF

    // Checksum values
    #define CKSUM_AC_KW 0xb3      // Checksum for AC ON command
    #define CKSUM_AC_ZERO_KW 0xc1 // Checksum for AC OFF command

