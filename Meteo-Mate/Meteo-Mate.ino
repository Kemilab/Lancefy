#include "wifi_mqtt.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

DHT dht22(2, DHT22);  //dht 22 sensor

float humidity = 0.0;  //var. for sensors
float temperature = 0.0;

#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 60          //Time ESP32 will go to sleep (in seconds) - 9.5 minutes

void setup() {
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
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  //enabled deep sleep
mesurements:
  if (WiFi.status() == WL_CONNECTED && client.connected() == true) {
    dht22.begin();
    delay(10000);
    GetDHT22();
    delay(1000);
    if(isnan(temperature) && isnan(humidity)){
      goto mesurements;
    }
    PublishData();
  } else {
    NeoError();
    Serial.println("MQTT server or WiFi error! Retrying...");
    if (WiFi.status() != WL_CONNECTED) {
      WiFiConnection();
      goto mesurements;
    } else {
      MQTTConnection();
      goto mesurements;
    }
  }
  Serial.println("Going to sleep");
  Serial.flush();
  PixelsOff();
  esp_deep_sleep_start();  //byeeeee
}

void GetDHT22() {
  if (client.connected() == true) {
    temperature = dht22.readTemperature();
    humidity = dht22.readHumidity();
  }
}

void PublishData() {
  String lineProtocol_1 = "temperature=" + String(temperature) + ",";
  lineProtocol_1 += "humidity=" + String(humidity);
  client.publish("esp/temp&humid", lineProtocol_1.c_str());
  delay(5000);  // 5s delay to make sure that mqtt published data
  Serial.println(lineProtocol_1);
}
void loop() {
  // put your main code here, to run repeatedly:
  return;
}
