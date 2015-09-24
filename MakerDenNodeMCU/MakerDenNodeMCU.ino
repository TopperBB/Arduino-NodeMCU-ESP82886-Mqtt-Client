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

const byte activeWifiConnection = 1;
const byte WifiConnections = 2;
int wifiRetry = 0;

const char* ssid[WifiConnections];
const char* password[WifiConnections];
const char* mqtt;
const char* devid;

enum lights {
	mqttConnected = D4, Sensing = D3
};

int leds[] = { mqttConnected, Sensing };

void setup() {
	Serial.begin(115200);
	delay(100);
	Serial.println();

	LedsInit();
	GetConfigFromEEPROM();
	MqttInit(leds[0]);
	MatrixInit();
	OneWireInit();

	WiFi.begin(ssid[activeWifiConnection], password[activeWifiConnection]);

	while (WiFi.status() != WL_CONNECTED && wifiRetry < 20) {
		delay(500);
		Serial.print(".");
		wifiRetry++;
	}

	Serial.println();
	Serial.println("IP address");
	Serial.println(WiFi.localIP());

}

void LedsInit() {
	for (int i; i < (sizeof(leds) / sizeof(int)); i++) {
		pinMode(leds[i], OUTPUT);
		digitalWrite(leds[i], LOW);
	}
	digitalWrite(leds[0], HIGH);
}

void loop() {
	if (WiFi.status() == WL_CONNECTED) {
		if (timeStatus() == timeNotSet) {
			Serial.println("get time");
			setSyncProvider(ntpUnixTime);
			setSyncInterval(60 * 60);
		}
		MqttConnect();
	}
	else { 
		delay(1000); 
	}

	GetTempReading();
	GetLightReading();

	MqttLoop();
}

void GetTempReading() {
	digitalWrite(leds[1], HIGH);

	float reading = GetTemperature();
	MqttPublish(reading, "temp", "c");

	digitalWrite(leds[1], LOW);

	float result = (float)((int)(reading * 10)) / 10;

	ScrollString(String(result), 16);
}

void GetLightReading() {
	digitalWrite(leds[1], HIGH);

	int r = analogRead(A0);
	float reading = (int)((float)r / 10.24f);  // convert to a percentage

	delay(50);
	digitalWrite(leds[1], LOW);

	SetDisplayBrightness((byte)reading);

	MqttPublish(reading, "light", "l");
}

void SetDisplayBrightness(byte lvl) {
	lvl = (lvl % 100) / 15;
	SetBrightness(lvl);
}




