/******************************************************************************************
* Copyright 2017 Ideetron B.V.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************************/
/****************************************************************************************
* File:     Nexus_pinger.ino
* Author:   Adri Verhoef & Gerben den Hartog for the original Nexus Sketch
* Company:  Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
* Created on:         13-09-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
*
* Description
*
* This firmware demonstrates a LoRaWAN pinger which send a message each minute.
****************************************************************************************/

#include "paperinode.h"
#include "lorawan_def.h"
#include "DS2401.h"

sLoRaWAN  lora;               //  See the Nexus_Lorawan.h file for the settings 

void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  Serial.begin(9600);

  while(DS_Read(&(lora.OTAA.DevEUI[0])) == false) {}
  printStringAndHex("DS2401 DEV EUI: ", &(lora.OTAA.DevEUI[0]), 8);
}

void loop(){ 
}


void printStringAndHex(const char *String, uint8_t *data, uint8_t n){
  uint8_t i;
  Serial.print(String);
  Serial.flush();
  Serial.print(n, DEC);
  Serial.print(" bytes; ");
  
  for( i = 0 ; i < n ; i++)  {    // Print the data as a hexadecimal string
    // Print single nibbles, since the Hexadecimal format printed by the Serial.Print function does not print leading zeroes.
    Serial.print((unsigned char) ((data[i] & 0xF0) >> 4), HEX); // Print MSB first
    Serial.print((unsigned char) ((data[i] & 0x0F) >> 0), HEX); // Print LSB second
    Serial.print(' ');
    Serial.flush();
  }
  Serial.println();
}
