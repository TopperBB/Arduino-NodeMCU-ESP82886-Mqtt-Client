# Maker Den on NodeMCU 

Implementation of the Maker Den Project and schema streaming sensor data over MQTT

This project implemented on the NodeMCU V2.0 (also known as V1.0) on the ESP8266, ESP-12E chip using the Arduino Firmware

##Physical Board

1. [NodeMCU v2 - Lua based ESP8266 development kit](http://tronixlabs.com/wireless/esp8266/nodemcu-v2-lua-based-esp8266-development-kit)
2. [BMP180 Barometric Pressure Sensor](http://tronixlabs.com/sensors/altitude/bmp180-barometric-pressure-sensor-board/)
3. [Adafruit Mini 8x8 LED Matrix w/I2C Backpack](http://tronixlabs.com/display/led/matrix/adafruit-mini-8x8-led-matrix-w-i2c-backpack-red-australia/)
4. 1 x [Light Dependent Resistor](http://tronixlabs.com/sensors/light/ldr/light-dependent-resistor/)
5. 1 x 10k resistor

![schematic](https://github.com/MakerDen/Maker-Den-Arduino-and-NodeMCU-ESP8266/blob/master/MakerDenNodeMCU/Fritzing/NodeMCU%20MQTT%20Board_bb.jpg)

    
## Software Requirements

1. On Windows, Mac and Linux you will need to install the latest [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx)
2. Arduino IDE 1.6.5 or better
  * Board Manager: NodeMCU V1.0 (ESP-12E Module) (required for the NodeMCU V2.0 physical board)
  * Board Manager http://arduino.esp8266.com/staging/package_esp8266com_index.json - As at July 2015 use the staging Board Manager with the V1.0 board

##Arduino on ESP8266
[Arduino on ESP8266 Project](https://github.com/esp8266/Arduino)

Add NodeMCU to Arduino IDE

1. Add Additional Board Manager URLs: File -> Preferences.  Add http://arduino.esp8266.com/stable/package_esp8266com_index.json 
2. Restart Arduino IDE
3. Arduino IDE 1.6.6 or greater: Tools -> Board -> Board Manager -> Search ESP8266


## IoT Dashboard
The IoT Dashboard allows you to visualise the data streamed to Azure. 

![IoT Dashboard](https://github.com/MakerDen/IoT-Maker-Den-NETMF/blob/master/MakerDen/Lab%20Code/IoTDashboard.JPG)

You can install the IoT Dashboard from [here](http://iotmakerdendashboard.azurewebsites.net/install/publish.htm).  Note, you will need to allow to run from unknown publisher.
