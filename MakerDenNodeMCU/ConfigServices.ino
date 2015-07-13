#include <EEPROM.h>

void GetConfigFromEEPROM(){
  EEPROM.begin(512);
  char* data;
  int length;
  
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  int address = 2;
  
  length = word(EEPROM.read(0), EEPROM.read(1));
  data = new char[length];
  
  for (address = 2; address < length + 2; address++) {
    data[address-2] = EEPROM.read(address);
  }
  data[address-2] = '\0';
  
  JsonObject& root = jsonBuffer.parseObject(data);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  ssid = new char[StringLen(root["SSID"]) + 1];
  password = new char[StringLen(root["Password"]) + 1];
  mqtt = new char[StringLen(root["Mqtt"]) + 1];
  devid = new char[StringLen(root["DevId"]) + 1];


  strcpy((char*)ssid, root["SSID"]);
  strcpy((char*)password, root["Password"]);
  strcpy((char*)mqtt, root["Mqtt"]);
  strcpy((char*)devid, root["DevId"]);
}

