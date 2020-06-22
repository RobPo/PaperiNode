/*
  1) Create a Grayscale picture with a resolution of 148x70 pixel
  2) Go to https://littlevgl.com/image-to-c-array and upload this image
  3) Fillout Name: "pic", select color format: "Indexed 4 colors", Output format "C array", Convert!
  4) Open pic.c in an editor and select the complete array-stream, excluding only the first 4 line swith color definition 
  5) Insert the image bytestream in the file progmen.h
  6) Use the function saveToFlash to store the picture in the external flash memory 
*/
#include "lorapaper.h"
#include "spi_flash.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.

void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  pinMode(SPI_FLASH_CS,  OUTPUT);
  digitalWrite(SPI_FLASH_CS,  HIGH);

  epd.begin();                        // Turn ON & initialize 1.1" EPD
  epd.saveImgToFlash(ADDR_PIC5);       // Save an image define in progmem.h to external Flash
  epd.loadFromFlash(ADDR_PIC5, 0);       // Load an image from external flash
  epd.update();                     
}

void loop(){ 
}
