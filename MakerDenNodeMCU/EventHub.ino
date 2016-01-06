#include <WiFiClientSecure.h>
#include "sha256.h"
#include "Base64.h"


// START: Azure Evet Hub settings
char KEY[] = "gCc53qmEdeAYsE3HgnCCV06JrEVe6J+2czkRSL9ySUQ=";
const char* KEY_NAME = "RPiDG";
const char* HOST = "MakerDen.azure-devices.net";
const char* END_POINT = "/devices/RPiDG/messages/events?api-version=2015-08-15-preview";
const int Mode = 0;
// END: Azure Evet Hub settings

const char* TARGET_URL = "/devices/RPiDG";

//// START: Azure Evet Hub settings
//char KEY[] = "Z2aUC4jbHD3aphn8pIMmOhiqf9/ns7o/SvPSixKKCEY=";
//const char* KEY_NAME = "D1";
//const char* HOST = "makerdenww-ns.servicebus.windows.net";
//const char* END_POINT = "/ehdevices/publishers/nodemcu/messages";
//const int Mode = 1;
//// END: Azure Evet Hub settings

int expire = 1511104241;  // Expires GMT: Sun, 19 Nov 2017 15:10:41 GMT.  Todo: add expiry window - eg now plus 2 days...

String fullSas;

int limit = 0;

int eventHubMsgCount = 0;

WiFiClientSecure tlsClient;

void InitialiseAzure(){
  if (Mode == 0) {
    InitialiseIotHub();
  }
  else {
    InitialiseEventHub();
  }
}

void InitialiseIotHub(){
  String url = URLEncode(HOST) + URLEncode(TARGET_URL);
  fullSas =  CreateIoTHubSas(url);
}

void InitialiseEventHub() {
  String url = URLEncode("https://")  + URLEncode(HOST) +  URLEncode(END_POINT);
  fullSas = CreateEventHubSas(url);
}

void ConnectToEventHub() {
  delay(1000); // give network connection a moment to settle
	if (!tlsClient.connect(HOST, 443)) {      // Use WiFiClientSecure class to create TLS connection
		Serial.println("tls host connection failed");
		delay(5000);
	}
	else {
		Serial.println("tls host connected");
	}
}

String CreateIoTHubSas(String url){  
  String stringToSign = url + "\n" + expire;

  // START: Create signature
  //https://raw.githubusercontent.com/adamvr/arduino-base64/master/examples/base64/base64.ino
  
  int keyLength = sizeof(KEY);
  
  int decodedKeyLength = base64_dec_len(KEY, keyLength);
  char decodedKey[decodedKeyLength];  //allocate char array big enough for the base64 decoded key
  
  base64_decode(decodedKey, KEY, keyLength);  //decode key
  
  Sha256.initHmac((const uint8_t*)decodedKey, decodedKeyLength);
  Sha256.print(stringToSign);  
  char* sign = (char*) Sha256.resultHmac();

  // END: Create signature
  
  // START: Get base64 of signature
  int encodedSignLen = base64_enc_len(HASH_LENGTH);
  char encodedSign[encodedSignLen];
  base64_encode(encodedSign, sign, HASH_LENGTH); 

  
  // SharedAccessSignature
  return "sr=" + url + "&sig="+ URLEncode(encodedSign) + "&se=" + expire;
  // END: create SAS  
}

String CreateEventHubSas(String url){  
  // START: Create SAS
  
  // https://azure.microsoft.com/en-us/documentation/articles/service-bus-sas-overview/
  // Where to get secods since the epoch: local service, SNTP, RTC

  String stringToSign = url + "\n" + expire;

  // START: Create signature
  Sha256.initHmac((const uint8_t*)KEY, 44);
  Sha256.print(stringToSign);
  
  char* sign = (char*) Sha256.resultHmac();
  int signLen = 32;
  // END: Create signature

  // START: Get base64 of signature
  int encodedSignLen = base64_enc_len(signLen);
  char encodedSign[encodedSignLen];
  base64_encode(encodedSign, sign, signLen);   
  // END: Get base64 of signature

  // SharedAccessSignature
  return "sr=" + url + "&sig="+ URLEncode(encodedSign) + "&se=" + expire +"&skn=" + KEY_NAME;
  // END: create SAS
}

//http://hardwarefun.com/tutorials/url-encoding-in-arduino
String URLEncode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    String encodedMsg = "";

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}

void PublishToEventHub(String data) {
	// https://msdn.microsoft.com/en-us/library/azure/dn790664.aspx

	String request = String("POST ") + END_POINT + " HTTP/1.1\r\n" +
		"Host: " + HOST + "\r\n" +
		"Authorization: SharedAccessSignature " + fullSas + "\r\n" +
		"Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" +
		"Content-Length: " + data.length() + "\r\n\r\n" +
		data;

	if (!tlsClient.connected()) { ConnectToEventHub(); }

  if (!tlsClient.connected()) { return; }

	// Serial.println(request);
	tlsClient.print(request);

	String response = "";
	String chunk = "";
  limit = 0;

	do {
		if (tlsClient.connected()) {
      delay(5);  // give esp8266 firmware networking some time
			chunk = tlsClient.readStringUntil('\n');
			response += chunk;
		}
   limit++;
	} while (chunk.length() > 0 && limit < 100);

  Serial.print(limit);
	Serial.print(" Message ");
	Serial.print(eventHubMsgCount++);
	Serial.print(" Response code: ");

	if (response.length() > 12) { Serial.println(response.substring(9, 12)); }
	else { Serial.println("unknown"); }
}




