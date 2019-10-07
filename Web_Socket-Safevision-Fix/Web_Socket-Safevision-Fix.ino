#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial
#define pir     D8
#define magnet  D6

#include <SocketIoClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

int Smagnet, Spir = 0;
int Dpir, Dmagnet;

const size_t capacity = JSON_OBJECT_SIZE(3);
DynamicJsonDocument doc(capacity);

void readsensor() {
  Spir = digitalRead(pir);  // Sensor pir
  if (Spir == 1) {
    Dpir = 1;
    Serial.print("Read digital input: ");
    Serial.println(Dpir);
  } else {
    Dpir = 0;
    Serial.print("Read digital input: ");
    Serial.println(Dpir);
  }

  boolean btnState = digitalRead(magnet); // Sensor Magnet
  if (btnState != LOW) {
    Serial.println("The Door Is OPEN");
    Dmagnet = 1;
  } else {
    Dmagnet = 0;
  }
}

void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}

void setup() {
  pinMode(pir, INPUT_PULLUP);      // GPIO 5 Pir Sensor
  pinMode(magnet, INPUT_PULLUP);       // GPIO 4 used Magnet Sensor
  
    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

     pinMode(D2, OUTPUT);
     digitalWrite(D2, !digitalRead(D2));

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("MEDOKAN BARU IV NO 12", "widyastuti");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.on("sensor_1_push", event);
    webSocket.begin("167.71.214.196",6500);
//    webSocket.begin("192.168.0.13",80,"/websocket");
    // use HTTP Basic Authorization this is optional remove if not needed
//    webSocket.setAuthorization("username", "password");
}

void loop() {
    webSocket.loop();
    
    readsensor();
    
    doc["id"] = "A54565";
    doc["pintu"] = Dmagnet;
    doc["motion"] = Dpir;
    char output[128];
    serializeJson(doc, output);
    
    Serial.println("Motion: "+String(Dpir));
    Serial.println( "Door: "+ String(Dmagnet));
    webSocket.emit("sensor_1_push", output);
    
    delay(1000);
}
