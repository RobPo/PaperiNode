/****************************************************************************************
* This example demonstrates a simple weatherforcest demo. The supercap voltage v_scap is 
* measured minutely while the ATmega328p processor is in deep sleep all remaining time. 
* Triggering is done via external RTC to minimize current consumption during deep sleep 
* phase. IF the voltage is charged above a certain limit (ie 4.2V) and a max. number of 
* allowed Lora syncs is not exceeded, a download of is triggered from the TTN console
* and the recieved content is printed on the Epaper Screen.
/****************************************************************************************
* File:               WeatherForecastExample.ino
* Author:             Robert Poser
* Created on:         19-12-2019
* Supported Hardware: LoraPaper (with RFM95, PV cell, supercap & 1.1" EPD)
* 
* Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for 
* more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power
* 
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************************/

#include "paperinode.h"
#include "mcp7940.h"
#include "DS2401.h"
#include "spi_flash.h"
#include "RFM95.h"
#include "LoRaMAC.h"
#include "lorawan_def.h"
#include "Cayenne_LPP.h"
#include "PL_microEPD44.h"

/******** GLOBAL VARIABLES ****************************************************************/
sAPP    app;                  // Application variable
sLoRaWAN  lora;               // See the lorapaper.h file for the settings 
sTimeDate TimeDate;           // RTC time and date variables
LORAMAC lorawan (&lora);      // Initialize the LoRaWAN stack.  
CayenneLPP LPP(&(lora.TX));   // Initialize the Low Power Protocol functions
PL_microEPD epd(EPD_CS, EPD_RST, EPD_BUSY);    //Initialize the EPD.

volatile bool RTC_ALARM = false;         // Interrupt variable
uint16_t v_scap, ndr, sync_max = 10, d; // V_Supercap, counter of failed downloads & max_syncs

/********* INTERRUPTS *********************************************************************/
ISR(INT1_vect) {                // Interrupt vector for the alarm of the MCP7940 Real Time 
  RTC_ALARM = true;             // Clock. Do not use I2C functions or long delays
}                               // here.

ISR(TIMER1_COMPA_vect) {        // Interrupt vector for Timer1 which is used to time the Join  
  lora.timeslot++;              // and Receive windows for timeslot 1 and timelsot 2 Increment 
}                               // the timeslot counter variable for timing the

/********* MAIN ***************************************************************************/
void setup(void) {  
  analogReference(EXTERNAL);          // use AREF for reference voltage
  SPI.begin();                        // Initialize the SPI port
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
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
  epd.printText("Syncing with TTN...", 16, 20, 1);
  epd.update();                       // Update EPD
  epd.end();                          // to save power...
  digitalWrite(RFM_NSS, LOW);         // to save power...
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
  
  mcp7940_init(&TimeDate, app.LoRaWAN_message_interval);   // Generate minutely interrupt 
  RTC_ALARM = true;
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC. 
        RTC_ALARM = false;             // Clear the boolean.
        
        mcp7940_reset_minute_alarm(app.LoRaWAN_message_interval);        
        mcp7940_read_time_and_date(&TimeDate);    
          
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        if (v_scap >= 640) {           // Proceed only if (Vscap > 4,2V)--> DEFAULT!
          if (++app.Counter%2) {       // Every two hours... 
            if (app.Counter>=4)        // Change wakeup from minutely to every 2hours
                app.LoRaWAN_message_interval=58;
   
            LPP.clearBuffer();         // Form a payload according to the LPP standard to 
            LPP.addDigitalOutput(0x00, app.Counter);
            LPP.addAnalogInput(0x00, v_scap*3.3/1023*4);

            SPI.begin();
            SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);             // Turn ON EPD without refresh to save power
            lorawan.init();               // Init the RFM95 module

            lora.RX.Data[6] = 25;         // Dummy value to check lateron if downlink data was received
            lorawan.LORA_send_and_receive();  // LoRaWAN send_and_receive
            if (lora.RX.Data[6] == 25)    // Downlink data received?
                ndr++;                    // if not increase nodatareceived (ndr) counter                      
            
            epd.printText("Current Weather ", 1, 2, 1);         // Header line

            String leadingHour = "";
            if(lora.RX.Data[3] < 10) {
              leadingHour = " ";
            }
            String leadingMinute = "";
            if(lora.RX.Data[4] < 10) {
              leadingMinute = "0";
            }
            epd.printText(leadingHour + String(lora.RX.Data[3]) + ":" + leadingMinute + String(lora.RX.Data[4]), 110, 2, 1);
    
            String leadingTemp = "";
            if(int8_t(lora.RX.Data[0]) > -1 && int8_t(lora.RX.Data[0]) < 10) {
              leadingTemp = " ";
            }
            epd.printText(leadingTemp + String(int8_t(lora.RX.Data[0])), 11, 16, 3);  // Temperature
            epd.printText("o", 53, 12, 2);
            epd.printText("C", 65, 16, 3);
    
            String leadingHumi = "";
            if(int8_t(lora.RX.Data[1]) < 10) {
              leadingHumi = " ";
            }
            epd.printText(leadingHumi + String(uint8_t(lora.RX.Data[1])), 11, 44, 3);  // Humidity
            epd.printText("%", 65, 44, 3);

            uint8_t icon = uint8_t(lora.RX.Data[5]); // Hotti Weather Forecast Icons
            switch (icon) {
              case 1:
                epd.drawBitmapLM(87, 15, wIcon_01, 24, 24);
                break;
              case 2:
                epd.drawBitmapLM(87, 15, wIcon_02, 24, 24);
                break;
              case 3:
                epd.drawBitmapLM(87, 15, wIcon_03, 24, 24);
                break;
              case 4:
                epd.drawBitmapLM(87, 15, wIcon_04, 24, 24);
                break;
              case 9:
                epd.drawBitmapLM(87, 15, wIcon_05, 24, 24);
                break;
              case 10:
                epd.drawBitmapLM(87, 15, wIcon_06, 24, 24);
                break;
              case 11:
                epd.drawBitmapLM(87, 15, wIcon_07, 24, 24);
                break;
              case 13:
                epd.drawBitmapLM(87, 15, wIcon_08, 24, 24);
                break;
              case 50:
                epd.drawBitmapLM(87, 15, wIcon_09, 24, 24);
                break;
            }
            
            epd.fillRectLM(87 , 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(92 , 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(97 , 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(102, 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(107, 41, 4, 1, EPD_BLACK);
            uint8_t r1 = uint8_t(lora.RX.Data[7])  / 5;
            uint8_t r2 = uint8_t(lora.RX.Data[8])  / 5;
            uint8_t r3 = uint8_t(lora.RX.Data[9])  / 5;
            uint8_t r4 = uint8_t(lora.RX.Data[10]) / 5;
            uint8_t r5 = uint8_t(lora.RX.Data[11]) / 5;
            if(r1 > 20)
              r1 = 20;
            if(r2 > 20)
              r2 = 20;
            if(r3 > 20)
              r3 = 20;
            if(r4 > 20)
              r4 = 20;
            if(r5 > 20)
              r5 = 20;
           
            epd.fillRectLM(87 , 39, 4, r1, EPD_BLACK);
            epd.fillRectLM(92 , 39, 4, r2, EPD_BLACK);
            epd.fillRectLM(97 , 39, 4, r3, EPD_BLACK);
            epd.fillRectLM(102, 39, 4, r4, EPD_BLACK);
            epd.fillRectLM(107, 39, 4, r5, EPD_BLACK);
                
            epd.printText("V " + String(v_scap*3.3/1023*2, 1), 115, 20, 1); // Plot last known voltage
            epd.printText("U " + String(app.Counter/2+1)      , 115, 30, 1); // Plot how many syncs have been tried
            epd.printText("D " + String(app.Counter/2+1 - ndr), 115, 40, 1); // Plot how many downlinks were empty
       
            epd.update();                                        // Send the framebuffer and do the update
            epd.end();                                           // To save power...
            digitalWrite(RFM_NSS, LOW);                          // To save power...
            SPI.endTransaction();                                // To save power...
            SPI.end();                                           // To save power...
          }
        }
      } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}
