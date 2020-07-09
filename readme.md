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


How To Use
-------------------

### Installation

Please download and start the latest Arduino IDE. Select "Pololu A-star 328pb, 5V/16MHz" in the menu Tools/Board; if not available please add to File>Preferences>Additional Boards Manager URL the following source "https://files.pololu.com/arduino/package_pololu_index.json". Now please download and import the examples of this repository.

### Hardware hookup

To program PaperiNode you will need to connect a FTDI programmer (USB to Serial, available for ~3€ e.g. at eBay) to the exposed pins on the bottom side. The most reliable version is to solder a pin row and connect everything on a breadboard. Alternatively, its possible to connect the programmer  directly to the bottom pins (if needed in combination with a slight pressure) or to 3D-print your own pogo-pin based clamp (see below):
![Programming the device](https://user-images.githubusercontent.com/21104467/87086258-9eb3f000-c231-11ea-9206-a208e11fee20.png)


Examples
-------------------
This is the place to get started! Ready to use examples can be found **[here](https://github.com/RobPo/paperinode/tree/master/examples)**.

Following demo projects are for your inspiration! What will you implement with PaperiNode? Tell us, we’ll love to add your project here!

![weatherforecast](https://user-images.githubusercontent.com/21104467/71322107-f3756080-24c3-11ea-96c5-fdd6a71fff85.jpg)

License Information
-------------------

This library is **open source**!

Libraries used in this sketch are based on the LoRaWAN stack from IDEETRON/NEXUS, for more infos please check this great source: https://github.com/Ideetron/Nexus-Low-Power

Libraries used in this sketch around the ePaper and the Example sketches are created by Robert Poser, Jun 20th 2020, Dresden/Germany. 

Released under GNU Lesser General Public License, either version 3 of the License, or (at your option) any later version, check license.md for more information.

We invested time and resources providing this source code, please support open source hardware and software @Ideetron, @Adafruit, @Watterott and others.

If you like this project please [follow us on Twitter](https://twitter.com/paperino_io).
Having problems or have awesome suggestions? Contact us: paperino.display@gmail.com.
