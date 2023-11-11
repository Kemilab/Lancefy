#include "secrets.h"
#include "HardwareSerial.h"
#include "esp32-hal.h"
#include "sys/_types.h"
#include "WiFiType.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "NeoPixel.h"
//times
//unsigned long start = millis();
//unsigned long delayTimeWiFi = 10000;  //10 sec //millis is causing a crash (stack smashing?)

//WiFi
const char *ssid = WiFissid;
const char *password = WiFipassword;

//MQTT
const char *mqtt_broker = broker;
const char *mqtt_username = MQTT_username;
const char *mqtt_password = MQTT_password;
const int mqtt_port = MQTT_port;
WiFiClient espClient;
PubSubClient client(espClient);

void WiFiConnection() {
  WiFi.begin(ssid, password);
  /* if (millis() - start < delayTimeWiFi) {
    ESP.restart();
  }*/
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i <= 1;) {
      NeoConnect();
      i++;
    }
    delay(500);
    Serial.println("Connecting to WiFi");
  }
  Serial.println("WiFi connected!");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.localIP());
  }
}
