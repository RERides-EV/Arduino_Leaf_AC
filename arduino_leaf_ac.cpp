/*  AC Controls from Arduino
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

#define AC_OUTPUT_PIN 1       // This is default pin for Serial1. Code uses a combination of writing to Serial1 and AC_OUTPUT_PIN to function. 
                              // If different serial port is used check for variable and serial in code

#define AC_INPUT_PIN 2        // This pin should be defined in main code and commented out here
#define DUMMY_PIN 3           // Not sure of dummy pin. Maybe not dummy, but assist with monitoring?
#define AC_ON_CMD 0xb3        // Command byte for AC ON
#define AC_OFF_CMD 0xb2       // Command byte for AC OFF
#define DEFAULT_AC_KW 0x0c    // 0x0c commands 1.5kW. Current default for ON/OFF but could be passed as same or different variable?
#define AC_ZERO_KW 0x00       // 0x00 commands 0kW
#define CKSUM_AC_KW 0xb3      // not sure of quick calculation yet; looks like AC_ON+0x16?
#define CKSUM_AC_ZERO_KW 0xc1 // 0kW checksum is always 0xc1


static int brklen = 14; // 13 bits + 1 bit for break delimiter
static uint8_t sync = 0x55;
static uint8_t addr = 0xfb;
static uint8_t AC_KW = DEFAULT_AC_KW  // ox0c commands 1.5kW
//static uint8_t AC_ZERO_KW = 0x00  // 0x00 commands 0kW
//static uint8_t CKSUM_AC_KW = 0xb3  // not sure of quick calculation yet; looks like AC_ON+0x16?
//static uint8_t CKSUM_AC_ZERO_KW = 0xc1 // always for AC_KW at 0kW
static uint8_t buf[8] = {AC_OFF_CMD, AC_ZERO_KW, 0x00, 0x90, 0xFF, 0x00, 0x00, 0x00};  
static uint8_t cksum = 0xb7;
static int baud = 19200;
static uint16_t brk = (brklen*1000000/baud); // brk in microseconds

void setupAC() {
  pinMode(AC_OUTPUT_PIN, OUTPUT);
  pinMode(AC_INPUT_PIN, INPUT_PULLUP);
  pinMode(DUMMY_PIN, OUTPUT);
  
  // Initialize USB serial for debugging
  Serial.begin(9600);
  Serial.print("Break time (us): ");
  Serial.println(brk);
  Serial.println("AC module initialized");
  
  // Initialize Serial1 for data transmission after break signal
  Serial1.begin(baud);
}

void updateAC(bool acEnabled) {                      //  TODO: Add variable maybe AC_KW to be passed, determined somehow
//  int ac_input_status = digitalRead(AC_INPUT_PIN); //  Determination if AC should be enabled is from main Aruino code and passed via updateAC(digitalRead(AC_INPUT_PIN))

//  if (uint8_t some_variable_for_power)
//      AC_KW = some variable
        
  if (acEnabled) {
    buf[0] = AC_ON_CMD;
    buf[1] = AC_KW;
    cksum = CKSUM_AC_KW;
    Serial.println("Compressor On Command");
  } else {
    buf[0] = AC_OFF_CMD;
    buf[1] = AC_ZERO_KW;
    cksum = CKSUM_AC_ZERO_KW;
    Serial.println("Compressor Off Command");
  }

  // Send LIN frame
  sendLINframe();
}

// Private helper function to send LIN bus commands

static void sendLINframe() {
  // Signal start of transmission
  digitalWrite(DUMMY_PIN, HIGH);
  
  // Generate LIN break signal manually
  digitalWrite(AC_OUTPUT_PIN, LOW);
  delayMicroseconds(brk);
  digitalWrite(AC_OUTPUT_PIN, HIGH);

  // Small delay after break signal
  delayMicroseconds(50);

  // Send LIN frame data using Serial1
  Serial1.begin(baud);  // Reinitialize before each transmission IS THIS NECESSARY?
  Serial1.write(sync);
  Serial1.write(addr);

  // Data bytes
  for (int i = 0; i < 8; i++) {
    Serial1.write(buf[i]); // Last byte   Serial1.write(buf[7]);
  }
  Serial1.write(cksum);
  
  // Wait for transmission to complete
  Serial1.flush();
  
  // Debug output
  Serial.print("Buffer[1] (Power Level): 0x");
  Serial.println(buf[1], HEX);
  
  // Add delay between transmissions
  delay(200);
}
