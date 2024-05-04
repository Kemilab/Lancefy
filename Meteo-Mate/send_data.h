#include "stdlib.h"
#include "HardwareSerial.h"
/************************************************************************
  LIBRARIES
************************************************************************/

#include "esp32-hal.h"
#include "WString.h"
#include "WiFiType.h"
#include "secrets.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <ArduinoHttpClient.h>
#include <SimpleTimer.h>

/************************************************************************
  SECRETS
************************************************************************/

const char* ssid = WiFissid;
const char* password = WiFipassword;
int port = PORT;
const char* key = ACCESS_TOKEN;
const char* server = ADDRESS;

/**********************************************************************
  VARS
***********************************************************************/

struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  float dewPoint;
  float heatIndex;
  float absHum;
  int uvIndex;
  unsigned long localTime;
};

SensorData data;
JsonDocument sensorData;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, port);
SimpleTimer reboteWiFi(20000);
int statusCode = 0;

void start_wifi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    delay(1000);
    if (reboteWiFi.isReady()) {
      Serial.println(F("WiFi failed! Restart!"));
      ESP.restart();
    }
  }
  Serial.print("SSID is" + String(WiFi.SSID()));
}

void sendData() {
  while (statusCode != 200) {
    JsonObject sensorData_0 = sensorData.add<JsonObject>();
    sensorData_0["temperature"] = data.temperature;
    sensorData_0["humidity"] = data.humidity;
    sensorData_0["pressure"] = data.pressure;
    sensorData_0["dewPoint"] = data.dewPoint;
    sensorData_0["heatIndex"] = data.heatIndex;
    sensorData_0["absHum"] = data.absHum;
    sensorData_0["uvIndex"] = data.uvIndex;
    String output;
    serializeJson(sensorData, output);
    Serial.println("Making a POST request");
    String postData = output;
    Serial.print(output);
    client.beginRequest();
    String url = "http://";
    url += ADDRESS;
    url += ":";
    url += String(PORT);
    url += "/api/v1/";
    url += ACCESS_TOKEN;
    url += "/telemetry";
    Serial.print(url);
    client.post(url.c_str());
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", postData.length());
    client.beginBody();
    client.print(postData);
    client.endRequest();
    statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
  }
}