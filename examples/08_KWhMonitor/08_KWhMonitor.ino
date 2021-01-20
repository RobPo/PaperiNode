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

volatile bool RTC_ALARM = false;        // Interrupt variable
uint16_t v_scap, ndr;   
uint16_t kWh[7];
uint8_t counter; 

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
  
  pinMode(SW_TFT, OUTPUT);            // Switch for V_Scap
  pinMode(DS2401, OUTPUT);            // Authenticating IC DS2401p+
  pinMode(RFM_NSS, OUTPUT);           // RFM95W NSS = CS
  pinMode(SPI_FLASH_CS, OUTPUT);      // External SPI flash
  digitalWrite(DS2401, HIGH);         // to save power...
  digitalWrite(SW_TFT, HIGH);         // to save power...
  digitalWrite(SPI_FLASH_CS, HIGH);   // to save power...

  epd.begin();                        // Turn ON & initialize 1.1" EPD
  epd.loadFromFlash(ADDR_PIC1, 0);    // Load an pic from external flash
  epd.saveFBToFlash(ADDR_FRAMEBUFFER);// And save it to external framebuffer on flash
  epd.update();                       // Update EPD
  epd.end();                          // to save power...
  digitalWrite(RFM_NSS, LOW);         // to save power...
  flash_power_down();                 // To save power...
  SPI.endTransaction();               // to save power...
  SPI.end();                          // to save power...
 
  I2C_init();                         // Initialize I2C pins
  mcp7940_init(&TimeDate, app.LoRaWAN_message_interval);   // Generate minutely interrupt 

  v_scap = analogRead(A7);            // 1st Dummy-read which always delivers strange values...(to skip)
  RTC_ALARM = true;
}

void loop(){ 
    if(RTC_ALARM == true){             // Catch the minute alarm from the RTC.       
        mcp7940_reset_minute_alarm(app.LoRaWAN_message_interval);        
        mcp7940_read_time_and_date(&TimeDate);    
          
        digitalWrite(SW_TFT, LOW);     // Turn ON voltage divider 
        delay(1);                      // To stabilize analogRead
        v_scap = analogRead(A7);       // Measure V_scap
        digitalWrite(SW_TFT, HIGH);    // Turn OFF voltage divider 
  
        if (v_scap >= 589) {           // Proceed only if (Vscap > 3,8V)--> DEFAULT for 1.5F!
          if (++app.Counter%2) {       // Every two hours... 
               app.LoRaWAN_message_interval=58;

            LPP.clearBuffer();         // Form a payload according to the LPP standard to 
            LPP.addDigitalOutput(0x00, app.Counter);
            LPP.addAnalogInput(0x00, v_scap*3.3/1024*2);

            SPI.begin();
            SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
            epd.begin(false);             // Turn ON EPD without refresh to save power
            lorawan.init();               // Init the RFM95 module

            delay(50);
            lora.RX.Data[6] = 25;         // Dummy value to check lateron if downlink data was received
            lorawan.LORA_send_and_receive();  // LoRaWAN send_and_receive
            if (lora.RX.Data[6] == 25)    // Downlink data received?
                ndr++;                    // if not increase nodatareceived (ndr) counter                      

            epd.loadFromFlash(ADDR_FRAMEBUFFER);        // Load last image to pre-buffer
            epd.printText("kWh-Monitor ", 1, 2, 1);         // Header line
            
            String leadingDay = "";
            if (lora.RX.Data[0] < 10)
              leadingDay = "0";

            String leadingMonth = "";
            if (lora.RX.Data[1] < 10)
              leadingMonth = "0";  
            epd.printText(leadingDay + String(lora.RX.Data[0]) + "." + leadingMonth + String(lora.RX.Data[1])+ ".", 70, 2, 1);

            String leadingHour = "";
            if(lora.RX.Data[2] < 10) 
              leadingHour = " ";
            
            String leadingMinute = "";
            if(lora.RX.Data[3] < 10) 
              leadingMinute = "0";
            epd.printText(leadingHour + String(lora.RX.Data[2]) + ":" + leadingMinute + String(lora.RX.Data[3]), 110, 2, 1);

            counter = (counter+1)%7;
            kWh[counter] = lora.RX.Data[4];
            char PufferChar[4];
            dtostrf(kWh[counter]/100.0, 3, 1,PufferChar);
            epd.printText(String(PufferChar) + "kWh", 5, 16, 3);  // kWh
            //epd.printText(" 10kWh", 5, 16, 3);  // kWh

            dtostrf(kWh[counter]/100*0.3, 3, 1,PufferChar);
            epd.printText(String(PufferChar) + "Eur", 5, 44, 3);  // kWh
            //epd.printText("3.1Eur", 5, 44, 3);  // kWh
 
 
            epd.fillRectLM(107, 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(112, 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(117, 41, 4, 1, EPD_BLACK);
            epd.fillRectLM(122, 41, 4, 1, EPD_BLACK);
            uint8_t r1 = kWh[0];
            uint8_t r2 = kWh[1];
            uint8_t r3 = kWh[2];
            uint8_t r4 = kWh[3];
            epd.fillRectLM(107, 39, 4, r1, EPD_BLACK);
            epd.fillRectLM(112, 39, 4, r2, EPD_BLACK);
            epd.fillRectLM(117, 39, 4, r3, EPD_BLACK);
            epd.fillRectLM(122, 39, 4, r4, EPD_BLACK);
          
            epd.printText("V " + String(v_scap*3.3/1024*2, 1), 115, 20, 1); // Plot last known voltage
            epd.printText("U " + String(app.Counter/2+1)      , 115, 30, 1); // Plot how many syncs have been tried
            epd.printText("D " + String(app.Counter/2+1 - ndr), 115, 40, 1); // Plot how many downlinks were empty

            epd.saveFBToFlash(ADDR_FRAMEBUFFER);
            if (v_scap > 640)                                   // IF V_scap > 4.2V
              epd.update();                                     // Trigger a 900ms default update (4GL)
            else
              epd.update(EPD_UPD_MONO);                         // Trigger a 450ms low power update (2GL)

            epd.end();                                           // To save power...
            digitalWrite(RFM_NSS, LOW);                          // To save power...
            flash_power_down();                                  // To save power...
            SPI.endTransaction();                                // To save power...
            SPI.end();                                           // To save power...
          }
        }
        RTC_ALARM = false;             // Clear the boolean.
      } else
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);      // To save power... 
}