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
