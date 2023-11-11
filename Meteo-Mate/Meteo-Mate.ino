#include "wifi_mqtt.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <DHT.h>
//dht 22 sensor
//DHT dht22(2, DHT22);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Trying to run WiFi setup");
  delay(100);
  WiFiConnection();
}

void loop() {
  // put your main code here, to run repeatedly:
}
