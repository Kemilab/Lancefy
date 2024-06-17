/************************************************************************
  LIBRARIES
************************************************************************/

#include "stdlib.h"
#include "HardwareSerial.h"
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
const String firmwareVersion = FW_VERSION;
const String stationName = STATION_NAME;
const String stationLocation = STATION_LOCATION;

/**********************************************************************
  VARS
***********************************************************************/

struct SensorData {
  int temperature;
  int humidity;
  int pressure;
  int dewPoint;
  int heatIndex;
  int absHum;
  int uvIndex;
  int luxLevel;
  float wind_speed;
  unsigned long localTime;
};

SensorData data;
JsonDocument sensorData;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, port);
SimpleTimer reboteWiFi(20000);
int statusCode = 0;
int tries = 0;

void start_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (reboteWiFi.isReady()) {
      Serial.println(F("WiFi failed! Restart!"));
      ESP.restart();
    }
  }
  Serial.print("SSID is" + String(WiFi.SSID()));
}

void sendData() {
  if (tries < 10 && statusCode != 200) {
    JsonObject sensorData_0 = sensorData.add<JsonObject>();
    sensorData_0["temperature"] = data.temperature;
    sensorData_0["humidity"] = data.humidity;
    sensorData_0["pressure"] = data.pressure;
    sensorData_0["dewPoint"] = data.dewPoint;
    sensorData_0["heatIndex"] = data.heatIndex;
    sensorData_0["absHum"] = data.absHum;
    sensorData_0["uvIndex"] = data.uvIndex;
    sensorData_0["luxLevel"] = data.luxLevel;
    sensorData_0["wind"] = data.wind_speed;
    sensorData_0["firmware_version"] = firmwareVersion;
    sensorData_0["stationName"] = stationName;
    sensorData_0["stationLocation"] = stationLocation;
    
    String output;
    serializeJson(sensorData, output);
    Serial.println("Making a POST request");
    String postData = output;
    Serial.println(output);
    client.beginRequest();
    String url = "http://";
    url += ADDRESS;
    url += ":";
    url += String(PORT);
    url += "/api/v1/";
    url += ACCESS_TOKEN;
    url += "/telemetry";
    Serial.println(url);
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
    tries++;
  }else{
    ESP.restart();
  }
}