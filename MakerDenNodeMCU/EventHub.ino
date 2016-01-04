#include <WiFiClientSecure.h>
#include "sha256.h"
#include "Base64.h"


// START: Azure Evet Hub settings
const char* KEY = "Z2aUC4jbHD3aphn8pIMmOhiqf9/ns7o/SvPSixKKCEY=";
const char* KEY_NAME = "D1";
const char* HOST = "makerdenww-ns.servicebus.windows.net";
const char* END_POINT = "/ehdevices/publishers/nodemcu/messages";
// END: Azure Evet Hub settings

int expire = 1511104241;
String fullSas;

int eventHubMsgCount = 0;

WiFiClientSecure tlsClient;

void InitialiseEventHub(){  
  String url = EncodeUrl("https://" + (String)HOST + (String)END_POINT);
  fullSas = CreateSas(url);
  ConnectToEventHub();
}

void ConnectToEventHub(){    
    if (!tlsClient.connect(HOST, 443)) {      // Use WiFiClientSecure class to create TLS connection
      Serial.println("connection failed");
      return;
  }
}

String EncodeUrl(String url){
  url.replace(":", "%3A");
  url.replace("/", "%2F");

  return url;
}

String CreateSas(String url){  
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
  String encodedSas = (String) encodedSign;
  
  // Naive URL encode
  encodedSas.replace("=", "%3D");
  // Serial.println(encodedSas);
  
  // END: Get base64 of signature

  // SharedAccessSignature
  return "sr=" + url + "&sig="+ encodedSas + "&se=" + expire +"&skn=" + KEY_NAME;
  // END: create SAS
}

void PublishToEventHub(String data){
  // https://msdn.microsoft.com/en-us/library/azure/dn790664.aspx
  
  String request = String("POST ") + END_POINT + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Authorization: SharedAccessSignature " + fullSas + "\r\n" +                
               "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" + 
               "Content-Length: " + data.length() + "\r\n\r\n" +
               data;
  
  // Serial.println(request);
  tlsClient.print(request);
  
  String response = "";
  String chunk = "";
  
  do {
    if (tlsClient.connected()) {
      chunk = tlsClient.readStringUntil('\n');
      response += chunk;
    }
  } while (chunk.length() > 0);
  
  Serial.print("Message ");
  Serial.print(eventHubMsgCount++);
  Serial.print(" Response code: ");
  
  if (response.length() > 12) { Serial.println(response.substring(9, 12)); }
  else { Serial.println("unknown"); }
}




