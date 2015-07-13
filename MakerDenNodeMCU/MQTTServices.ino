#include <MQTT.h>           // https://github.com/Imroy/pubsubclient
#include <PubSubClient.h>   // https://github.com/Imroy/pubsubclient

WiFiClient wclient; 

PubSubClient client(wclient);
String mqttNamespace = "gb/";

int sendCount = 0;
const int BufferLen = 256;
char buffer[BufferLen];


void MqttInit(){
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
  client.loop();  
}


void MqttPublish(double reading, const char * type, const char * unit, int ledPublish) {
  int length;
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  digitalWrite(ledPublish, LOW); 
  
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
  
  digitalWrite(ledPublish, HIGH);

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
