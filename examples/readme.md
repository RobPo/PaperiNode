Example: 01_SerialPrintDevEUI
-------------------
Prints the individual Device EUI address to the serial console. 

Example: 02_Hello World
-------------------
Shows the first text on the epd screen.

Example: 03_SaveImgToFlash
-------------------
Shows how to store upto 7 pictures on the flash memory.

Example: 04_MeasVScap
-------------------
Example which prints the measured voltage of the supercap storage device on the epd screen.

Example: 05_Minimal
-------------------
Demonstrates a simple counter, being updated everytime if there is sufficient energy harvested. The supercap voltage v_scap is measured minutely while the ATmega328pb processor is in deep sleep all remaining time. Triggering is done via external RTC to minimize current consumption during deep sleep phase. IF the voltage is charged above a certain limit (ie 4.2V), an image update is triggered. 

Example: 06_WeatherForecast
-------------------
Adds a LoRa up- and downlink on top of the previous template; the received payload is then shown on the screen.
