/*  Leaf AC Compressor Controls from Arduino
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

Controls values for the Air Conditioner compressor:
  Input on pin 2 - HIGH = compressor ON, LOW = compressor OFF
  Output on pin 1 - TX pin of Serial1, used for LIN bus communication
  buf[1] controls compressor power (0-3kW):
    - 0x00: 0kW (OFF)
    - 0x05: 1kW
    - 0x0c: ~1.5kW
    - 0x12: 2kW
    - 0x16: 3kW

LIN Protocol Implementation:
  1. Generate break signal by pulling line low
  2. Send sync byte (0x55)
  3. Send ID byte (0xFB)
  4. Send 8 data bytes
  5. Send checksum

Originally developed by "abetanco" 
https://openinverter.org/forum/viewtopic.php?p=75831#p75831

This code is in the Public Domain
*/

#include "arduino_leaf_ac.h"

// Private variables
static int brklen = 14;      // 13 bits + 1 bit for break delimiter
static uint8_t sync = 0x55;
static uint8_t addr = 0xfb;
static uint8_t currentPowerLevel = DEFAULT_AC_KW;
static uint8_t buf[8] = {AC_OFF_CMD, AC_ZERO_KW, 0x00, 0x90, 0xFF, 0x00, 0x00, 0x00};  
static uint8_t cksum = 0xb7;
static int baud = 19200;
static uint16_t brk = (brklen*10000/baud); // brk in microseconds

static unsigned long lastStatusReport = 0;
static const unsigned long STATUS_INTERVAL = 5000; // 5 seconds

// Private function declaration
static void sendLINframe(int outputPin, int debugPin);

void initializeAC(int outputPin, int inputPin, int debugPin) {
    pinMode(outputPin, OUTPUT);
    pinMode(inputPin, INPUT_PULLUP);
    pinMode(debugPin, OUTPUT);
    
    // Initialize USB serial for debugging
    Serial.begin(9600);
    Serial.print("Break time (us): ");
    Serial.println(brk);
    Serial.println("AC module initialized");
    
    // Initialize Serial1 for data transmission after break signal
    Serial1.begin(baud);
}

void updateAC(bool acEnabled, int outputPin, int debugPin) {
    if (acEnabled) {
        buf[0] = AC_ON_CMD;
        buf[1] = currentPowerLevel;
        cksum = CKSUM_AC_KW;
        Serial.println("Compressor On Command");
    } else {
        buf[0] = AC_OFF_CMD;
        buf[1] = AC_ZERO_KW;
        cksum = CKSUM_AC_ZERO_KW;
        Serial.println("Compressor Off Command");
    }

    // Send LIN frame
    sendLINframe(outputPin, debugPin);

    // Periodic status reporting
    unsigned long currentMillis = millis();
    if (currentMillis - lastStatusReport >= STATUS_INTERVAL) {
        Serial.println("Status Report:");
        Serial.print("AC Enabled: ");
        Serial.println(acEnabled ? "YES" : "NO");
        Serial.print("Buffer[1] (Power Level): 0x");
        Serial.println(buf[1], HEX);    
        lastStatusReport = currentMillis;
    }
}

void setACPower(uint8_t powerLevel) {
    currentPowerLevel = powerLevel;
}

static void sendLINframe(int outputPin, int debugPin) {
    // Signal start of transmission
    digitalWrite(debugPin, HIGH);
    
    // Generate LIN break signal manually
    digitalWrite(outputPin, LOW);
    delayMicroseconds(brk);
    digitalWrite(outputPin, HIGH);

    // Small delay after break signal
    delayMicroseconds(50);

    // Send LIN frame data using Serial1
    Serial1.begin(baud);
    Serial1.write(sync);
    Serial1.write(addr);

    // Data bytes
    for (int i = 0; i < 8; i++) {
        Serial1.write(buf[i]);
    }
    Serial1.write(cksum);
    
    // Wait for transmission to complete
    Serial1.flush();

    // Add delay between transmissions
    delay(200);
}
