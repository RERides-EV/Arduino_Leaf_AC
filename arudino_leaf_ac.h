/*  AC Controls from Arduino - Header File
    Copyright (C) 2025 David Durazzo
    for RERides EV
*/

#ifndef ARDUINO_LEAF_AC_H
#define ARDUINO_LEAF_AC_H

#include <Arduino.h>

// Pin definitions - can be changed in main code if needed
#define AC_OUTPUT_PIN 1       // Must be Serial1 TX pin for proper LIN communication
#define AC_INPUT_PIN 2        // Input pin to detect when AC should be ON/OFF
#define DUMMY_PIN 3           // Used for debugging/triggering

// AC command bytes
#define AC_ON_CMD 0xb3        // Command byte for AC ON
#define AC_OFF_CMD 0xb2       // Command byte for AC OFF

// AC power settings
#define AC_ZERO_KW 0x00       // 0kW (OFF)
#define AC_POWER_LOW 0x05     // 1kW
#define DEFAULT_AC_KW 0x0c    // ~1.5kW
#define AC_POWER_MED 0x12     // 2kW
#define AC_POWER_HIGH 0x16    // 3kW

// Checksum values
#define CKSUM_AC_KW 0xb3      // Checksum for AC ON command
#define CKSUM_AC_ZERO_KW 0xc1 // Checksum for AC OFF command

// Function declarations
void setupAC();
void updateAC(bool acEnabled);
void setACPower(uint8_t powerLevel);
void sendLINframe ();
