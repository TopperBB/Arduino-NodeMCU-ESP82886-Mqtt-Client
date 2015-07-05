#include <ArduinoJson.h>
#include <EEPROM.h>


const char* ssid = "NCW";
const char* password = "malolos5459";
//const char* mqtt = "gloveboxAE.cloudapp.net";
const char* mqtt = "raspberrypi";
const char* devid = "node1";

const int BufferLen = 510;  // max eprom length, minus 2 for data length
char buffer[BufferLen];
int length;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["SSID"] = ssid;
  root["Password"] = password;  
  root["Mqtt"] = mqtt;
  root["DevId"] = devid;
  
  length = root.printTo(buffer, BufferLen);
  root.prettyPrintTo(Serial);
  
  EEPROM.write(0, highByte(length));
  EEPROM.write(1, lowByte(length));
  for (int address = 2; address < length + 2; address++){
    EEPROM.write(address, buffer[address-2]);
  }
  EEPROM.commit();

  ssid = "hello";
  Serial.println(ssid);
}

void loop() {

  getconfiguration();
  delay(2000);
  
}

void getconfiguration(){
  StaticJsonBuffer<200> jsonBuffer;
  int address = 2;
  length = word(EEPROM.read(0), EEPROM.read(1));
  for (address = 2; address < length + 2; address++) {
    buffer[address-2] = EEPROM.read(address);
  }
  buffer[address-2] = '\0';
  Serial.println(buffer);

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

  
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(mqtt);
  Serial.println(devid);
}


