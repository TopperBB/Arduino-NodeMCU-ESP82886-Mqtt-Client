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


const char* ssid;
const char* password;
const char* mqtt;
const char* devid;


int leds[] = {D4, D1};


void setup() {
  Serial.begin(115200);
  delay(100);  
  Serial.println(); 

  LedsInit();
  GetConfigFromEEPROM();
  MqttInit(); 

  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("IP address: " + WiFi.localIP());

  setSyncInterval(60*60);
  setSyncProvider(ntpUnixTime);
}

void LedsInit(){
  for (int i; i < (sizeof(leds)/sizeof(int)) ; i++){
      pinMode(leds[i], OUTPUT);
      digitalWrite(leds[i], LOW); 
  }
}

void loop() {
  if (!MqttConnect()) {
    Serial.println("mqtt connect retry");    
    delay(500);      
  }
  else {
    GetLightReading();
  }

  MqttLoop();

  delay(450);
}

void GetLightReading() {
  digitalWrite(leds[1], HIGH); 
  int r = analogRead(A0);
  float reading = (int)((float)r / 10.24f);
  delay(50);    
  digitalWrite(leds[1], LOW); 
  
  MqttPublish(reading, "light", "l", leds[0]);
}




