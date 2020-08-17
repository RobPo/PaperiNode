#include "paperinode.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    	//Initialize the EPD.

void setup(void) {  
  analogReference(EXTERNAL);                   	// use AREF for reference voltage
  SPI.begin();                                 	// Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  epd.begin();                                 	// Turn ON & initialize 1.1" EPD
  // printText(String text, int x, int y, int size);
  epd.printText("Hello World!", 35, 20, 1);    	// Add text to framebuffer
  epd.update();                     			// Send framebuffer to EPD and trigger update
}

void loop(){ 
}
