/*
  1) Create a Grayscale picture with a resolution of 148x70 pixel (e.g. with Gimp)
  2) Go to https://littlevgl.com/image-to-c-array and upload this image
  3) Fillout Name: "pic", select color format: "Indexed 4 colors", Output format "C array", Convert!
  4) Open pic.c in an editor and select the complete array-stream, excluding only the first 4 lines with color definition 
  5) Insert the image bytestream in the file progmen.h
  6) Use the function saveToFlash to store the picture from progmem into the external flash memory 
*/
#include "paperinode.h"
#include "spi_flash.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.

void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));

  pinMode(SPI_FLASH_CS,  OUTPUT);
  digitalWrite(SPI_FLASH_CS,  HIGH);

  epd.begin();                        // Turn ON & initialize 1.1" EPD
  epd.saveImgToFlash(ADDR_PIC1);      // Save an image define in progmem.h to external Flash
  epd.loadFromFlash(ADDR_PIC1, 0);    // Load an image from external flash
  epd.update();                     
}

void loop(){ /*                       // Run an image loop with 2secs delay
  epd.loadFromFlash(ADDR_PIC2, 0);       
  epd.update();                     
  delay(2000);
  epd.loadFromFlash(ADDR_PIC3, 0);       
  epd.update();                     
  delay(2000); */
}
