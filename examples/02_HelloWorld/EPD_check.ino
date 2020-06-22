#include "paperinode.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.

void setup(void) {  
  analogReference(EXTERNAL);                   // use AREF for reference voltage
  SPI.begin();                                 // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  epd.begin();                                 // Turn ON & initialize 1.1" EPD
  epd.printText("Hello World!", 35, 20, 1);     
  epd.update();                     
}

void loop(){ 
}
