#include "esp32-hal.h"
#include "WString.h"
#include "WiFiType.h"
#include "secrets.h"
#include <WiFi.h>
#include <ArduinoHttpClient.h>

const char* ssid = WiFissid;
const char* password = WiFipassword;
int port = PORT;
const char* key = ACCESS_TOKEN;
const char* server = ADDRESS;


WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, port);
void start_wifi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi");
    delay(10000);
  }
  Serial.print("SSID is" + String(WiFi.SSID()));
}