#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Sangsaka's iPhone";
const char* password = "123456789";

const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(6) + 100;
DynamicJsonDocument doc(capacity);

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

void scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
 
  }
}

void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}



void setup() {
  // put your setup code here, to run once:
//  pinMode(2,INPUT);
  Serial.begin(115200);
  scanNetworks();
  connectToNetwork();
 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
 
//  WiFi.disconnect(true);
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
   int sensor = digitalRead(2);
   Serial.print("Sensor: ");
   Serial.println(sensor);
   
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
//    http.begin("http://172.20.10.9:3000/sensor/getById/5d30640321d4ccb2f1556132"); //Specify the URL
    http.begin("https://trafficnet.id/sensor/getById/5d3158dfe57bce652711d4cc"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
//        Serial.println(httpCode);
//        Serial.println(payload);
        
        deserializeJson(doc, payload);
        
        const char* data_0 = doc["data"][0]; // "0"
        
        const char* _id = doc["_id"]; // "5d30640321d4ccb2f1556132"
        const char* jenissensor = doc["jenissensor"]; // "alarm"
        const char* status_alarm = doc["status"]; // "1"
        const char* username = doc["username"]; // "sangsakawira"
        int _v = doc["__v"]; // 0

        Serial.print("Status Alarm: ");
        Serial.println(status_alarm);

        if(sensor == 0){
          Serial.println("Sensor Aman");
          http.begin("https://trafficnet.id/sensor/setById/5d3158dfe57bce652711d4c/0");
          http.GET();
        }else{
          Serial.println("Indikasi");
          http.begin("https://trafficnet.id/sensor/setById/5d3158dfe57bce652711d4c/1");
          http.GET();
        }
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(1000);
}
