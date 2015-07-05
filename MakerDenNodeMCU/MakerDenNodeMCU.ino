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
#include <EEPROM.h>


//Network
WiFiClient wclient; 
WiFiUDP udp; // NTP Time service

const char* ssid     = "";
const char* password = "";
const char* mqtt = "";
const char* devid = "";

// MQTT Related
const int BufferLen = 256;
char buffer[BufferLen];
String mqttNamespace = "gb/";
PubSubClient client(wclient);

char isoTime[20];

int length;
int sendCount = 0;

//IPAddress mqttBroker(191, 239, 72, 14); //gloveboxae
IPAddress mqttBroker(192, 168, 1, 17); //on RPiB

int builtin_led = BUILTIN_LED;  // On when publishing over mqtt
int publishLed = D1;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  getConfigFromEEPROM();

  Serial.println(mqtt);
  client.set_server(mqttBroker);
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

void getConfigFromEEPROM(){
  EEPROM.begin(512);
  
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  int address = 2;
  
  length = word(EEPROM.read(0), EEPROM.read(1));
  
  for (address = 2; address < length + 2; address++) {
    buffer[address-2] = EEPROM.read(address);
  }
  buffer[address-2] = '\0';
  
  JsonObject& root = jsonBuffer.parseObject(buffer);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  ssid = root["SSID"];
  password = root["Password"];
  mqtt = root["Mqtt"];
  devid = root["DevId"];
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
  
  root["Dev"] = "Node";
  root["Geo"] = "2011";  
  root["Type"] = type;
  root["Unit"] = unit;
  
  JsonArray& data = root.createNestedArray("Val");
  data.add(reading, 0);  // 2 is the number of decimals to print 
  data.add(ESP.getFreeHeap() , 0);  // 2 is the number of decimals to print   

  root["Utc"] = GetISODateTime(isoTime);
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


char* GetISODateTime(char * isoTime) {
  sprintf(isoTime, "%4d-%02d-%02dT%02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  return isoTime;
}

time_t inline ntpUnixTime ()
{
  int udpInited = udp.begin(123); // open socket on arbitrary port

  const char timeServer[] = "pool.ntp.org";  // NTP server

  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (! udpInited)
    return 0;

  // Clear received data from possible stray received packets
  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(timeServer, 123) // 123 is the NTP port
   && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
   && udp.endPacket()))
    return 0;       // sending request failed

  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150;   // poll every this many ms
  const byte maxPoll = 15;    // poll up to this many times
  int pktLen;       // received packet length
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;       // no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();  // NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv/8);

  // Discard the rest of the packet
  udp.flush();

  Serial.println("time: " + String(time - 2208988800ul));

  return time - 2208988800ul;   // convert NTP time to Unix time
}

