#include "paperinode.h"
#include "mcp7940.h"
#include "DS2401.h"
#include "LoRaMAC.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.
sTimeDate   TimeDate;                 // RTC time and date variables
volatile bool RTC_ALARM = false;      // Interrupt variable
uint16_t      v_scap, c;              // V_Supercap, counter 
bool t[24][12];

/********* INTERRUPTS *********************************************************************/
ISR(INT1_vect) {                // Interrupt vector for the alarm of the MCP7940 Real Time 
  RTC_ALARM = true;             // Clock. Do not use I2C functions or long delays
}                               // here.

/********* MAIN ***************************************************************************/
void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  Serial.begin(9600);
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  pinMode(DS2401, OUTPUT);            // Authenticating IC DS2401p+
  pinMode(RFM_NSS, OUTPUT);           // RFM95W NSS = CS
  pinMode(SPI_FLASH_CS, OUTPUT);      // External SPI flash
  digitalWrite(DS2401, HIGH);         // to save power...
  digitalWrite(SW_TFT, HIGH);         // to save power...
  digitalWrite(SPI_FLASH_CS, HIGH);   // to save power...

  epd.begin();                        // Turn ON & initialize 1.1" EPD
  SPI_Write(RFM_NSS, 0x01, 0x00);          // Switch RFM to sleep
  //epd.loadFromFlash(ADDR_PIC1, 0);    // Load an pic from external flash
  epd.saveFBToFlash(ADDR_FRAMEBUFFER);// And save it to external framebuffer on flash
  epd.update();                       // Update EPD
  epd.end();                          // to save power...
  digitalWrite(RFM_NSS, LOW);         // to save power...
  flash_power_down();                 // To save power...
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...
 
  I2C_init();                         // Initialize I2C pins
  mcp7940_init(&TimeDate, 1);   // Generate minutely interrupt 
  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
  RTC_ALARM = true;
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC. 
        mcp7940_reset_minute_alarm(5); 
        mcp7940_read_time_and_date(&TimeDate);    
         
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        if (v_scap >= 574) {           // Proceed only if (Vscap > 3,7V (640))--> DEFAULT!  
            c++;       
            SPI.begin();
            SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);
            epd.clear(EPD_BLACK);
            epd.drawBitmapLM(0, 18, wIcon_10, 24, 24);
            epd.drawBitmapLM(82, 6, wIcon_10, 24, 24);
            epd.drawBitmapLM(110, 20, wIcon_10, 24, 24);
            SPI_Write(RFM_NSS, 0x01, 0x00);          // Switch RFM to sleep
            epd.loadFromFlash(ADDR_FRAMEBUFFER);        // Load last image to pre-buffer

            String leadingHour = "";
            if(TimeDate.hours < 10) {
              leadingHour = " ";
            }
            String leadingMinute = "";
            if(TimeDate.minutes < 10) {
              leadingMinute = "0";
            }
            epd.printText(leadingHour + String(TimeDate.hours) + ":" + leadingMinute + String(TimeDate.minutes), 5, 10, 3);
            String leadingDay = "";
            if(TimeDate.day < 10) {
              leadingDay = " ";
            }
            String leadingMonth = "";
            if(TimeDate.month < 10) {
              leadingMonth = "0";
            }
            epd.printText(leadingDay + String(TimeDate.day) + "." + leadingMonth + String(TimeDate.month) + "." + String(TimeDate.year), 2, 50, 2);
            epd.printText("Debug", 117, 25, 1);
            epd.printText("V " + String(v_scap*3.3/1024*2, 1), 115, 35, 1); // Plot last known voltage
            epd.printText("C " + String(c)      , 115, 45, 1); // Plot how many syncs have been tried
                      
            epd.saveFBToFlash(ADDR_FRAMEBUFFER);
            if (v_scap > 680)                                   // IF V_scap > 4.4V
              epd.update();                                     // Trigger a 900ms default update (4GL)
            else
              epd.update(EPD_UPD_LOWP);                         // Trigger a 450ms low power update (2GL)

            epd.end();
            digitalWrite(RFM_NSS, LOW);                          // To save power...
            flash_power_down();                                  // To save power...
            SPI.endTransaction();                                // To save power...
            SPI.end();         
        }
        RTC_ALARM = false;             // Clear the boolean.
    } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}
