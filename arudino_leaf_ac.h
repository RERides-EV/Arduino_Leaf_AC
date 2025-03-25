/*  Leaf AC Compressor Controls from Arduino - Header File
    Copyright (C) 2025 David Durazzo
    for RERides EV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

Originally developed by "abetanco" 
https://openinverter.org/forum/viewtopic.php?p=75831#p75831

This code is in the Public Domain
*/

#ifndef ARDUINO_LEAF_AC_H
#define ARDUINO_LEAF_AC_H

#include <Arduino.h>

// EXTERNAL Pin declarations - MUST DEFINE IN .INO FILE
extern int AC_OUTPUT_PIN;       // Must be same as Serial1 TX pin for proper LIN communication
extern int AC_INPUT_PIN;        // Input pin to detect when AC should be ON/OFF
extern int DUMMY_PIN;           // Used for debugging/triggering


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
static void sendLINframe ();

#endif // ARDUINO_LEAF_AC_H
