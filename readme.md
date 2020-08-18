Solar powered E-Paper Node for LoRaWAN
===============================================================

Welcome to the docs! This is an Arduino Library for PaperiNode, a TTN-connected 1.1” E-Paper Node which is powered by ambient light and thus energy-autark. 


![paperinodes](https://user-images.githubusercontent.com/21104467/85378463-e13fb200-b53a-11ea-95d8-0eedb5c36a9c.png)
[*PaperiNode*](https://twitter.com/Paperino_io)

Hardware specification (**[Datasheets](https://github.com/RobPo/paperinode/tree/master/datasheets)**):
-----------------------
- MCU: ATmega328pb (16MHz, 32KB FLASH, 2KB SRAM)
- External RTC: MCP7940M
- LoRa Chip: RFM95W, Antenna EU_863_870 or u.FL connector for external antenna
- EPD: 1.1" Plastic Logic, 148x70pixel
- Flash: Winbond W25X40Cl, 4Mbit
- Low power design: Deep Sleep 2.4uA
- PV cells: IXYS SLMD121H04L
- Energy-harvesting PMIC: E-Peas AEM10941 w/ MPPT
- Storage device: EDLC supercap 400mF

### Revisions

![image](https://user-images.githubusercontent.com/21104467/87089413-b3df4d80-c236-11ea-80cd-ae3580d8e8bf.png)  
The second revision (black FPC) is the successor of the first edition (white FPC) and contains the following changes:
- Reduced deep sleep current
- SPI pins exposed on bottom side to connect external sensors etc.
- SPI flash added to store measurement data or upto seven pictures

How To Use
-------------------

### Installation

Please download and start the latest Arduino IDE. Select "Pololu A-star 328pb, 5V/16MHz" in the menu Tools/Board; if not available please add to File>Preferences>Additional Boards Manager URL the following source "https://files.pololu.com/arduino/package_pololu_index.json". Now please download and import the examples of this repository.

### Hardware hookup

To program PaperiNode you will need to connect a FTDI programmer (USB to Serial, available for ~3€ e.g. at eBay) to the exposed pins on the bottom side. The most reliable version is to solder a pin row and connect everything on a breadboard. Alternatively, its possible to connect the programmer  directly to the bottom pins (if needed in combination with a slight pressure) or to 3D-print your own pogo-pin based clamp (see below):
![Programming the device](https://user-images.githubusercontent.com/21104467/87086258-9eb3f000-c231-11ea-9206-a208e11fee20.png)

### E-Ink display library

Based on Adafruit GFX library this version enables the easy usage of the 1.1" EPD in combination with the MCU. Most work was done around coping with the limited available SRAM size. The following commands are available:

```C++
void begin();				// Initializes and clears the screen
void clear();				// Clears the framebuffer	
void printText(String text, int x, int y, int s);	// Write text to a defined position
void loadFromFlash(int address=ADDR_FRAMEBUFFER, bool toPreviousBuffer=true);		// Loads a previously stored pic from SPI flash
void fillRectLM(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);					// Draws a filled rectangle
void update(int updateMode=EPD_UPD_FULL);	// Trigger an update with the framebuffer content
void end(void);				// De-init the screen to save power

```




### Examples

Now its time to get started! Ready to use examples can be found **[here](https://github.com/RobPo/paperinode/tree/master/examples)**:

- 01_SerialPrintDevEUI > Prints the individual Device EUI address to the serial console. 
- 02_Hello World 	   > Shows the first text on the epd screen.
- 03_SaveImgToFlash    > Shows how to store upto seven pictures in the flash memory.
- 04_MeasVScap 		   > Example which prints the measured voltage of the supercap storage device on the epd screen.
- 05_Minimal 		   > Demonstrates a simple counter, being updated everytime if there is sufficient energy harvested. The supercap voltage v_scap is measured minutely while the ATmega328pb processor is in deep sleep all remaining time. Triggering is done via external RTC to minimize current consumption during deep sleep phase. IF the voltage is charged above a certain limit (ie 4.2V), an image update is triggered. 
- 06_WeatherForecast   > Based on the previous demo,  a LoRa up- and downlink is added; the received payload is then shown on the screen.

Power consumption
-------------------
![image](https://user-images.githubusercontent.com/21104467/87191015-dd11e380-c2f3-11ea-8748-3fb30a526311.png)
For this example the used energy is 0,7Joule (based on ABP with SF7 and 18byte payload within downlink). 

Projects
-------------------
Following demo projects are for your inspiration! What will you implement with PaperiNode? Tell us, we’ll love to add your project here!

![weatherforecast](https://user-images.githubusercontent.com/21104467/71322107-f3756080-24c3-11ea-96c5-fdd6a71fff85.jpg)

Where To Get
-------------------
PaperiNode is available at [tindie](https://www.tindie.com/products/robertposer/paperinode-solarpowered-e-paper-node-for-lorawan/)
![image](https://user-images.githubusercontent.com/21104467/87087892-2e5a9e00-c234-11ea-817c-b1f2ac832ec2.png)

License Information
-------------------

This library is **open source**!

Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power

Libraries used in this sketch around the ePaper and the Example sketches are created by Robert Poser, Jun 20th 2020, Dresden/Germany. 

Released under GNU Lesser General Public License, either version 3 of the License, or (at your option) any later version, check license.md for more information.

We invested time and resources providing this source code, please support open source hardware and software @Ideetron, @Adafruit, @Watterott, @Tindie and others.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
