#include "wifi_mqtt.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <DHT.h>
//dht 22 sensor
DHT dht22(2, DHT22);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Trying to run WiFi setup");
  delay(100);
  WiFiConnection();
  switch (WiFi.status() == WL_CONNECTED) {
    case 1:
      Serial.println("Trying to run MQTT setup");
      delay(100);
      MQTTConnection();
    default:;
  };
}

void GetData() {
  if (client.connected() == true) {
    delay(1000);
    float temperature = dht22.readTemperature();
    float humidity = dht22.readHumidity();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
