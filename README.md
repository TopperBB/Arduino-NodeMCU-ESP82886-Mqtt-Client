# Temperature, Pressure and Light Sensor Streaming over MQTT

## Platform

This project implemented on the NodeMCU V2.0 (also known as V1.0) on the ESP8266, ESP-12E chip using the Arduino Firmware

## Schema

Streams data in the following JSON formats

1. Maker Den. (MQTT Namespace gb/device/type)

    {"Dev":"DeviceId","Geo":"2011","Type":"temp","Unit":"c","Val":[27.30,25848],"Utc":"2015-12-06T23:06:08","Id":84}
    {"Dev":"DeviceId","Geo":"2011","Type":"kPa","Unit":"kPa","Val":[101.00,25824],"Utc":"2015-12-06T23:06:19","Id":85}
    {"Dev":"DeviceId","Geo":"2011","Type":"light","Unit":"l","Val":[99.00,25840],"Utc":"2015-12-06T23:06:19","Id":86}
    
2. Azure IoT Hub Field Gateway. (MQTT Namespace iothub/#)

    {"Dev":"DeviceId","Geo":"2011","Celsius":27,"hPa":1016,"Light":99,"Utc":"2015-12-06T23:07:04","Id":103}

##Physical Board

1. [NodeMCU v2 - Lua based ESP8266 development kit](http://tronixlabs.com/wireless/esp8266/nodemcu-v2-lua-based-esp8266-development-kit)
2. [BMP180 Barometric Pressure Sensor](http://tronixlabs.com/sensors/altitude/bmp180-barometric-pressure-sensor-board/)
3. [Adafruit Mini 8x8 LED Matrix w/I2C Backpack](http://tronixlabs.com/display/led/matrix/adafruit-mini-8x8-led-matrix-w-i2c-backpack-red-australia/)
4. 1 x [Light Dependent Resistor](http://tronixlabs.com/sensors/light/ldr/light-dependent-resistor/)
5. 1 x 10k resistor

![schematic](https://github.com/MakerDen/Maker-Den-Arduino-and-NodeMCU-ESP8266/blob/master/MakerDenNodeMCU/Fritzing/NodeMCU%20MQTT%20Board_bb.jpg)

    
## Software Requirements

1. On Windows, Mac and Linux you will need to install the latest [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx).
2. [Arduino IDE 1.6.5](https://www.arduino.cc/en/Main/Software) As at Dec, 2015, [Arduino 1.6.6 has several issues, so to stick with 1.6.5](http://esp8266.github.io/Arduino/versions/2.0.0/doc/installing.html)
3. As at Dec 2015, ESP8266 Board Manager 2.0.0 or better required.

##Arduino on ESP8266
[Arduino on ESP8266 Project](https://github.com/esp8266/Arduino)

Add NodeMCU to Arduino IDE

1. Add Additional Board Manager URLs: File -> Preferences.  Add http://arduino.esp8266.com/stable/package_esp8266com_index.json 
2. Restart Arduino IDE
3. Add ESP8266 Board: Tools -> Board -> Board Manager -> Search ESP8266 -> Install
4. Select NodeMUC Board: Tools -> Board -> NodeMCU 1.0 (ESP-12E module)
5. Set Port and Upload Speed: Tools.  Note, you may need to try different port speeds to sucessfully flash the device. Faster is better as each time you upload the code to the NodeMCU you are uploading the complete ROM not just your code.



##Project Files

1. SetEEPROMConfiguration.ino - Set your Wi-Fi SSID and password (up to two can be specified - ground pin 7 to select the second Wi-Fi), plus the device id, MQTT server name, and geo location.  Deploy this app to the NodeMCU to write configuration settings to EPROM
7. MakerDenNodeMCU.ino - This is the main application, it will read configuration setting from the EPROM, manages sensor readings and publishing to MQTT

##Subscripting to MQTT Data

Install [Mosquiotto](www.mosquitto.org) client tools from the download page.  Install Open SSL and PThread libraries as per set up instructions.

1. To subscribe to Maker Den data

    mosquitto_sub.exe -h YourMqttServer -t gb/# 
    
2. To subscribe to IoT Hub data

    mosquitto_sub.exe -h YourMqttServer -t iothub/#



## IoT Dashboard
The IoT Dashboard allows you to visualise the data streamed to a MQTT service running in an Azure Virtual Machine. 

![IoT Dashboard](https://github.com/MakerDen/IoT-Maker-Den-NETMF/blob/master/MakerDen/Lab%20Code/IoTDashboard.JPG)

You can install the IoT Dashboard from [here](http://iotmakerdendashboard.azurewebsites.net/install/publish.htm).  Note, you will need to allow to run from unknown publisher.
