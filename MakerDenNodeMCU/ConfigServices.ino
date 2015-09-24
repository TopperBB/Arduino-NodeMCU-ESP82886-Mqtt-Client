#include <EEPROM.h>

void GetConfigFromEEPROM() {
	EEPROM.begin(512);
	char* data;
	int length;

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0);
	StaticJsonBuffer<1000> jsonBuffer;
	int address = 2;

	length = word(EEPROM.read(0), EEPROM.read(1));
	data = new char[length];

	for (address = 2; address < length + 2; address++) {
		data[address - 2] = EEPROM.read(address);
	}
	data[address - 2] = '\0';

	JsonObject& root = jsonBuffer.parseObject(data);
	if (!root.success())
	{
		Serial.println("parseObject() failed");
		return;
	}

	mqtt = new char[StringLen(root["Mqtt"]) + 1];
	devid = new char[StringLen(root["DevId"]) + 1];

	strcpy((char*)mqtt, root["Mqtt"]);
	strcpy((char*)devid, root["DevId"]);

	for (int i = 0; i < WifiConnections; i++)
	{
		ssid[i] = new char[StringLen(root["SSID"][i]) + 1];
		password[i] = new char[StringLen(root["Password"][i]) + 1];

		strcpy((char*)ssid[i], root["SSID"][i]);
		strcpy((char*)password[i], root["Password"][i]);
	}
}

