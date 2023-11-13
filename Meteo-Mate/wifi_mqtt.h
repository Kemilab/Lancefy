#include "secrets.h"
#include "HardwareSerial.h"
#include "esp32-hal.h"
#include "sys/_types.h"
#include "WiFiType.h"
#include <WiFi.h>
#include "NeoPixel.h"
#include <PubSubClient.h>

//times
bool hasRun = false;

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
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  NeoConnect();
  delay(10000);
  if (WiFi.status() != WL_CONNECTED) {
    NeoError();
    delay(100);
    PixelsOff();
    ESP.restart();
  }
  Serial.println("WiFi connected!");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.localIP());
  }
}

void MQTTConnection() {
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
    String client_id = MQTT_username;
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("MQTT protocol changed state to up");
    } else {
      Serial.println("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
