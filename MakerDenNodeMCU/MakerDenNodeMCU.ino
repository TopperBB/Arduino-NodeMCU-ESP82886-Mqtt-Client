// Author: dglover@microsoft.com
// Date: July 2015
// Physical Board: NodeMCU v2 - Lua based ESP8266 development kit 
	// http://tronixlabs.com/wireless/esp8266/nodemcu-v2-lua-based-esp8266-development-kit
	// http://www.nodemcu.com/index_en.html
// Board Manager: NodeMCU V1.0 (ESP-12E Module) (required for the NodeMCU V2.0 physical board)
// Board Manager http://arduino.esp8266.com/staging/package_esp8266com_index.json - As at July 2015 use the staging Board Manager with the V1.0 board
// https://github.com/esp8266/Arduino
// see pins_arduino.h in the \variants\nodemcu directory

#include <Time.h>           // http://playground.arduino.cc/code/time - installed via library manager
#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Wifi management variables
const byte WifiConnections = 2;
int WifiIndex = -1;
unsigned long LastWifiTime = 0;
const int WifiTimeoutMilliseconds = 60000;  // 60 seconds
int WiFiConnectAttempts = 0;
int MQTTConnectionAttempts = 0;


const char* ssid[WifiConnections];
const char* password[WifiConnections];
const char* mqtt;
const char* devid;
const char* geo;

Adafruit_BMP085 bmp;

// Globals
float temperature;
int pressure;
int light;

enum lights {
	mqttConnected = D4, Sensing = BUILTIN_LED
};


int leds[] = { mqttConnected, Sensing };

void setup() {
	Serial.begin(9600);
	delay(100);
	Serial.println();

	LedsInit();
	GetConfigFromEEPROM();
	MqttInit(leds[0]);
	MatrixInit();
  bmp.begin();
}


void loop() {
	if (WiFi.status() == WL_CONNECTED) {
		if (timeStatus() == timeNotSet) {
      Serial.println(WiFi.localIP());
			setSyncProvider(ntpUnixTime);
			setSyncInterval(60 * 60);
		}
		MqttConnect();
	}
	else {
    InitWifi();
	}

	GetTempReading();
	GetLightReading();
  PublishIoTHub();

	MqttLoop();
}


void LedsInit() {
  for (int i; i < (sizeof(leds) / sizeof(int)); i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  digitalWrite(leds[0], HIGH);
}


void InitWifi(){
  if (WiFi.status() == WL_CONNECTED) {return;}  

  if (LastWifiTime >  millis() ) {
    delay(500);    
    return;
  }
  
  WifiIndex++;
  if (WifiIndex >= WifiConnections) {WifiIndex = 0;}

  Serial.println("trying " + String(ssid[WifiIndex]));

  WiFi.begin(ssid[WifiIndex], password[WifiIndex]);
  
  WiFiConnectAttempts++;
  LastWifiTime = millis() + WifiTimeoutMilliseconds;
}


// publish data in flattened schema for IoT Hub
void PublishIoTHub(){
  MqttPublish(temperature, pressure, light, geo);
}


void GetTempReading() {
	digitalWrite(leds[1], LOW);

  temperature = bmp.readTemperature();  
	MqttPublish(temperature, "temp", "c", geo);

  pressure = (int)((int)( bmp.readPressure() + 0.5) / 100);
  MqttPublish(pressure, "hPa", "hPa", geo);
  
  digitalWrite(leds[1], HIGH);

  int roundedTemp = (int)(temperature + 0.5);
  
  ScrollString(" " + String(roundedTemp) + "C " + String(pressure) + "hPa", 71); 
}


void GetLightReading() {
	digitalWrite(leds[1], LOW);

	int r = analogRead(A0);
	light = (int)((float)r / 10.24f);  // convert to a percentage
  MqttPublish(light, "light", "l", geo);

  digitalWrite(leds[1], HIGH);
  
  SetDisplayBrightness((byte)light);
}


void SetDisplayBrightness(byte lvl) {
	lvl = (lvl % 100) / 15;
	SetBrightness(lvl);
}





