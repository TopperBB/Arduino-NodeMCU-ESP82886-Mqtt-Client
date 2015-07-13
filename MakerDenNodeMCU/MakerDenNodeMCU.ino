// Author: dglover@microsoft.com
// Date: July 2015
// Physical Board: NodeMCU v2 - Lua based ESP8266 development kit 
    // http://tronixlabs.com/wireless/esp8266/nodemcu-v2-lua-based-esp8266-development-kit
    // http://www.nodemcu.com/index_en.html
// Board Manager: NodeMCU V1.0 (ESP-12E Module) (required for the NodeMCU V2.0 physical board)
// Board Manager http://arduino.esp8266.com/staging/package_esp8266com_index.json - As at July 2015 use the staging Board Manager with the V1.0 board
// https://github.com/esp8266/Arduino


#include <Time.h>           // http://playground.arduino.cc/code/time - installed via library manager
#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <MQTT.h>           // https://github.com/Imroy/pubsubclient
#include <PubSubClient.h>   // https://github.com/Imroy/pubsubclient
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>



//Network
WiFiClient wclient; 
WiFiUDP udp; // NTP Time service

const char* ssid;
const char* password;
const char* mqtt;
const char* devid;

// MQTT Related
const int BufferLen = 256;
char buffer[BufferLen];
String mqttNamespace = "gb/";
PubSubClient client(wclient);



int length;
int sendCount = 0;

int builtin_led = BUILTIN_LED;  // On when publishing over mqtt
int publishLed = D1;

void setup() {
  Serial.begin(115200);
  delay(100);  
  Serial.println();  
  
  GetConfigFromEEPROM();

  Serial.println(ssid);
  Serial.println(password);
  Serial.println(mqtt);
  Serial.println(devid);  

  client.set_server(mqtt);
  mqttNamespace += String(devid) + "/";
  
  pinMode(builtin_led, OUTPUT);
  pinMode(publishLed, OUTPUT);
  digitalWrite(builtin_led, LOW); 
  digitalWrite(publishLed, LOW); 

//  client.set_callback(callback);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  setSyncInterval(60*60);
  setSyncProvider(ntpUnixTime);
}

void loop() {
  if (!client.connected()) {
    Serial.println("mqtt connect");
      client.connect(String(ESP.getChipId()));
      // client.subscribe(TOPIC);
      delay(500);      
  }
  else 
  {
    GetLightReading();
  }

  client.loop();

  delay(400);
}


int StringLen(const char* source){
  int len = 0;
  while (source[len] != '\0' && len < 100) { len++; }
  return len;
}


void GetLightReading() {
  digitalWrite(publishLed, HIGH); 
  int r = analogRead(A0);
  float reading = (int)((float)r / 10.24f);
  
  digitalWrite(publishLed, LOW); 
  delay(100);
  
  Publish(reading, "light", "l");
}

void LedBlink(uint8_t pin) {
  digitalWrite(pin, HIGH); 
  delay(25);
  digitalWrite(pin, LOW);
  delay(25);
}

void Publish(double reading, const char * type, const char * unit) {
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  digitalWrite(builtin_led, LOW); 
  
  String TopicName = mqttNamespace + (String)type;
  char Topic[50];
  TopicName.toCharArray(Topic, 50, 0);
  
  root["Dev"] = devid;
  root["Geo"] = "2011";  
  root["Type"] = type;
  root["Unit"] = unit;
  
  JsonArray& data = root.createNestedArray("Val");
  data.add(reading, 0);  // 2 is the number of decimals to print 
  data.add(ESP.getFreeHeap() , 0);  // 2 is the number of decimals to print   

  root["Utc"] = GetISODateTime();
  root["Id"] = sendCount++;
  
  length = root.printTo(buffer, BufferLen);
  
  client.publish(Topic, buffer);    //http://knolleary.net/arduino-client-for-mqtt/api/#publish
  digitalWrite(builtin_led, HIGH);

}

// Callback function
void callback(const MQTT::Publish& pub) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.

  // Copy the payload to a new message
  MQTT::Publish newpub("outTopic", pub.payload(), pub.payload_len());
  client.publish(newpub);
}

// handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  Serial.println("topic: " + String(topic));
  Serial.println("len: " + String(length, DEC));
  // create character buffer with ending null terminator (string)

  for (i = 0; i < length  && i < BufferLen - 1; i++) {
    buffer[i] = payload[i];
  }

  buffer[i] = '\0';
  String msgString = String(buffer);
  Serial.println("msg: " + msgString);
}





