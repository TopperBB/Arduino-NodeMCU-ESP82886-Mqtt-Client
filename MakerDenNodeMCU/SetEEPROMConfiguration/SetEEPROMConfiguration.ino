#include <ArduinoJson.h>
#include <EEPROM.h>


const char* ssid[] = { "NCW", "dgTether" };
const char* password[] = { "malolos5459", "VisualStudio2005" };
const char* mqtt = "gloveboxAE.cloudapp.net";
const char* devid = "node01";
const char* geo = "-33.8745828,151.2067465";  // Sydney Town Hall

const int BufferLen = 510;  // max eprom length, minus 2 for data length
char buffer[BufferLen];
int length;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  //root["SSID"] = ssid;

  JsonArray& _ssid = root.createNestedArray("SSID");
  _ssid.add(ssid[0]);
  _ssid.add(ssid[1]);

  JsonArray& _password = root.createNestedArray("Password");
  _password.add(password[0]);
  _password.add(password[1]);

  root["Mqtt"] = mqtt;
  root["DevId"] = devid;
  root["Geo"] = geo;
  
  length = root.printTo(buffer, BufferLen);
  root.prettyPrintTo(Serial);
  
  EEPROM.write(0, highByte(length));
  EEPROM.write(1, lowByte(length));
  for (int address = 2; address < length + 2; address++){
    EEPROM.write(address, buffer[address-2]);
  }
  EEPROM.commit();
}

void loop() {

  getconfiguration();
  delay(2000);
  
}

void getconfiguration(){
  StaticJsonBuffer<1000> jsonBuffer;
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

  ssid[0] = root["SSID"][0];
  password[0] = root["Password"][0];
  mqtt = root["Mqtt"];
  devid = root["DevId"];
  geo = root["Geo"];
  
  Serial.println(ssid[0]);
  Serial.println(password[0]);
  Serial.println(ssid[1]);
  Serial.println(password[1]);
  Serial.println(mqtt);
  Serial.println(devid);
  Serial.println(geo);
}


