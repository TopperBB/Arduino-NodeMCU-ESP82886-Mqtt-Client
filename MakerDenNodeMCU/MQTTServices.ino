#include <MQTT.h>           // https://github.com/Imroy/pubsubclient
#include <PubSubClient.h>   // https://github.com/Imroy/pubsubclient

WiFiClient wclient; 

PubSubClient client(wclient);
String mqttNamespace = "gb/";
String mqttIoTHubNamespace = "gb/iothub";

int MqttConnectedLed;

int sendCount = 0;
const int BufferLen = 256;
char buffer[BufferLen];


void MqttInit(int ledMqttConnected){
  MqttConnectedLed = ledMqttConnected;
  client.set_server(mqtt);  
  mqttNamespace += String(devid) + "/";
//  client.set_callback(callback);
}

bool MqttConnect(){
  if (client.connected()) { 
    return true; 
  } 
  else 
  {
    Serial.println("mqtt connect");
    client.connect(String(ESP.getChipId()));

    return client.connected();
  }
}

void MqttLoop(){
  if (!client.connected()) { return; }
  client.loop();  
}

void MqttPublish(int temperature, int pressure, int light){
  if (!client.connected()) { 
    digitalWrite(MqttConnectedLed, HIGH);
    return; 
  }

  digitalWrite(MqttConnectedLed, LOW); 


  int length;
//  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  String TopicName = mqttIoTHubNamespace;
  char Topic[50];
  TopicName.toCharArray(Topic, 50, 0);
  
  root["Dev"] = devid;
  root["Geo"] = "2011";  
  root["Celsius"] = temperature;
  root["hPa"] = pressure;
  root["Light"] = light;
  
//  JsonArray& data = root.createNestedArray("Val");
//  data.add(reading, 2);  // 2 is the number of decimals to print 
//  data.add(ESP.getFreeHeap()); 

  root["Utc"] = GetISODateTime();
  root["Id"] = sendCount++;
  
  length = root.printTo(buffer, BufferLen);
  
  client.publish(Topic, buffer);    //http://knolleary.net/arduino-client-for-mqtt/api/#publish
  
}


void MqttPublish(double reading, const char * type, const char * unit) {

  if (!client.connected()) { 
    digitalWrite(MqttConnectedLed, HIGH);
    return; 
  }

  digitalWrite(MqttConnectedLed, LOW); 

  int length;
//  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  String TopicName = mqttNamespace + (String)type;
  char Topic[50];
  TopicName.toCharArray(Topic, 50, 0);
  
  root["Dev"] = devid;
  root["Geo"] = "2011";  
  root["Type"] = type;
  root["Unit"] = unit;
  
  JsonArray& data = root.createNestedArray("Val");
  data.add(reading, 2);  // 2 is the number of decimals to print 
  data.add(ESP.getFreeHeap()); 

  root["Utc"] = GetISODateTime();
  root["Id"] = sendCount++;
  
  length = root.printTo(buffer, BufferLen);
  
  client.publish(Topic, buffer);    //http://knolleary.net/arduino-client-for-mqtt/api/#publish
}

//// Callback function
//void callback(const MQTT::Publish& pub) {
//  // In order to republish this payload, a copy must be made
//  // as the orignal payload buffer will be overwritten whilst
//  // constructing the PUBLISH packet.
//
//  // Copy the payload to a new message
//  MQTT::Publish newpub("outTopic", pub.payload(), pub.payload_len());
//  client.publish(newpub);
//}

//// handles message arrived on subscribed topic(s)
//void callback(char* topic, byte* payload, unsigned int length) {
//  int i = 0;
//  Serial.println("topic: " + String(topic));
//  Serial.println("len: " + String(length, DEC));
//  // create character buffer with ending null terminator (string)
//
//  for (i = 0; i < length  && i < BufferLen - 1; i++) {
//    buffer[i] = payload[i];
//  }
//
//  buffer[i] = '\0';
//  String msgString = String(buffer);
//  Serial.println("msg: " + msgString);
//}
