#include <HTTPClient.h>
#include "secrets.h"

const char *serverAddress = server;
const char *API_KEY = key;
const char *serverPort = "8085";

HTTPClient http;
void sendData(const String &payload) {
  String serverPath = "http://" + String(serverAddress) + ":" + String(serverPort) + "/api/v1/" + String(API_KEY) + "/telemetry";
  http.begin(serverPath);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP response: " + response);
  } else {
    Serial.println("Error sending data to server. HTTP response code: " + String(httpResponseCode));
  }

  http.end();
}