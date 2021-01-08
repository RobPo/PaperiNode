
Example: 06_WeatherForecast
-------------------
### 1) Register PaperiNode at your own TTN Console

Now its time to add PaperiNode as new device to your own TheThingsNetwork account. After login at TTN, please go to 'APPLICATION' and 'DEVICES' and select '+register device' (if you have a fresh account at TTN you will need to add an application first with your favourite name). Please add for 'Device ID' a name of your choice and insert the 'Device EUI'. This 16bit unique identifier can be obtained from example 01_SerialPrintDevEUI in the examples folder, and once running can be copied from the serial console. After pressing 'Register' and selecting 'Settings' please change the 'Activation Method' to 'ABP' and disable the checkbox next to 'Frame Counter Checks'. You will now see the new generated keys, please copy 'Device Adress', 'Network Session Key' and the 'App Session Key' since they will be added into the source code lateron. Well done so far.

### 2) Schedule downloadable data at your TTN console with NodeRED

There are several ways to add data to the TTN console, which can then be downloaded to your node once it comes online the next time. In this example, the follwing shown Node-RED flow is implemented; you can continue to use it or build your own flow:

![noderead2](https://user-images.githubusercontent.com/21104467/71321637-c1accb80-24bc-11ea-906a-3cce3634e421.jpg)

The underlying code is located in the file 'nodeRED_weatherFlow.txt' withtin the exmaple directory and can be imported to NodeRED. You will need to update the IDs written in capital letters to your own setup.

Requesting a new weather API call is here triggered via a predefined timer (timer, i.e. every 2 hours) or after the node came online and just downloaded the present weather data (ttn uplink). With this setup we assure that there is always weather data available, no matter when the node came back online the last time. 

The underlying weather API from DarkSky.net does not accept new sign-ups since its recent joining to Apple (but the service continues till end of 2021). Temporarely you can use the provided link in the NodeRED flow, updated by the coordinates of your own location. In the meantime we are working an integrating an alternative weather API.

The orange box extract payload parses the received html code and extracts only the relevant weather data we are interested in. As a result, the data size decreases from several kilobytes down to less than ten bytes. This payload is now configured in ttn-downlink to be downloaded to our weather station when it is online next time.

![scheduled](https://user-images.githubusercontent.com/21104467/71321656-2a944380-24bd-11ea-957f-c4cb6a82c611.jpg)

### 3) Add the Encoder function to your TTN backend

Next we need to add an encoder function, which converts the weather data to a bytestream which can be sent during the next donwlink message. Please fill in the data from the file 'encoder.h' to the field in Application/Payload Format/Encoder.

### 4) Modify & Upload The Source Code

New please open the example '06_WeatherForecast' and select the tab 'lorawan_def.h'. Here you will need to add your previously generated keys from the TTN console:

![slora](https://user-images.githubusercontent.com/21104467/71319850-f1030e80-24a3-11ea-84f9-7d1ee86cc57c.jpg)

After compiling and successful upload you should be able to receive the first data! If not, please let me know!
