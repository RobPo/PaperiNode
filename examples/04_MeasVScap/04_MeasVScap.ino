#include "paperinode.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);     //Initialize the EPD.
uint16_t    v_scap;                             // V_Supercap, counter of failed downloads & max_syncs

void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  pinMode(A7, INPUT);                 // To measure V_Scap
  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  digitalWrite(SW_TFT, HIGH);         // to save power...

  epd.begin();                        // Turn ON & initialize 1.1" EPD

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
}

void loop(){       
    digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
    delay(1);                      // To stabilize analogRead
    v_scap = analogRead(A7);       // Measure V_scap
    digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
    epd.clear();
    epd.printText(String(float(v_scap*3.3/1023*2)), 10, 20, 2);  // Following lines look a bit complex; due to 
    epd.printText(String("V"), 60, 20, 2);  // Following lines look a bit complex; due to 
    epd.printText(String(v_scap), 10, 40, 2);  // Following lines look a bit complex; due to 
    epd.update();
    delay(2000);
}
