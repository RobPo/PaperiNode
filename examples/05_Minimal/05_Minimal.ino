#include "paperinode.h"
#include "mcp7940.h"
#include "DS2401.h"
#include "LoRaMAC.h"
#include "PL_microEPD44.h"

PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.
sTimeDate   TimeDate;                 // RTC time and date variables
volatile bool RTC_ALARM = false;      // Interrupt variable
uint16_t      v_scap, i;              // V_Supercap, counter 

/********* INTERRUPTS *********************************************************************/
ISR(INT1_vect) {                // Interrupt vector for the alarm of the MCP7940 Real Time 
  RTC_ALARM = true;             // Clock. Do not use I2C functions or long delays
}                               // here.

/********* MAIN ***************************************************************************/
void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));
  
  pinMode(7, INPUT);                 // To measure V_Scap
  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  pinMode(DS2401, OUTPUT);            // Authenticating IC DS2401p+
  pinMode(RTC_MFP, INPUT);            // RTC MCP7940
  pinMode(RFM_DIO0, INPUT);           // RFM95W DIO0
  pinMode(RFM_DIO1, INPUT);           // RFM95W DIO1
  pinMode(RFM_DIO5, INPUT);           // RFM95W DIO5
  pinMode(RFM_NSS, OUTPUT);           // RFM95W NSS = CS
  pinMode(SPI_FLASH_CS, OUTPUT);
  digitalWrite(DS2401, HIGH);         // to save power...
  digitalWrite(SW_TFT, HIGH);         // to save power...
  digitalWrite(SPI_FLASH_CS, HIGH);   // to save power...

  PCMSK1 = 0;                         // Disable Pin change interrupts  
  PCICR = 0;                          // Disable Pin Change Interrupts
  delay(10);                          // Power on delay for the RFM module
  I2C_init();                         // Initialize I2C pins
  flash_power_down();                 // To save power...
  sei();                              // Enable Global Interrupts 
 
  epd.begin();                        // Turn ON & initialize 1.1" EPD
  SPI_Write(RFM_NSS, 0x01, 0x00);     // Switch RFM to sleep
  epd.update();  
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)

  mcp7940_init(&TimeDate, 1);         // Generate minutely interrupt 
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC. 
        RTC_ALARM = false;             // Clear the boolean.
        
        mcp7940_reset_minute_alarm(1);        
        mcp7940_read_time_and_date(&TimeDate);    
          
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        if (v_scap >= 640) {           // Proceed only if (Vscap > 4,4V (640))--> DEFAULT!         
            SPI.begin();
            SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);
            SPI_Write(RFM_NSS, 0x01, 0x00);          // Switch RFM to sleep
            epd.printText(TimeDate.hours, 40, 10, 3);
            epd.update();
            epd.end();
            digitalWrite(RFM_NSS, LOW);                          // To save power...
            SPI.endTransaction();                                // To save power...
            SPI.end();         
        }
      } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}
