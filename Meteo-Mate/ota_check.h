#ifndef OTA_CHECK_H
#define OTA_CHECK_H

#include <WiFi.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <ArduinoHttpClient.h>
#include "secrets.h"

// Global variables
const char* serverAddress = ADDRESS;
const char* token = ACCESS_TOKEN;
int serverPort = PORT;
extern HttpClient client;

// Function to send telemetry data to ThingsBoard
void sendFirmwareState(const char* state, const char* error = nullptr, const char* title = nullptr, const char* version = nullptr) {
  StaticJsonDocument<200> doc;
  doc["fw_state"] = state;
  if (error != nullptr) {
    doc["fw_error"] = error;
  }
  if (title != nullptr) {
    doc["current_fw_title"] = title;
  }
  if (version != nullptr) {
    doc["current_fw_version"] = version;
  }

  String output;
  serializeJson(doc, output);

  String url = String("https://") + serverAddress + ":" + String(serverPort) + "/api/v1/" + token + "/telemetry";
  client.beginRequest();
  client.post(url.c_str());
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", output.length());
  client.beginBody();
  client.print(output);
  client.endRequest();
}

// Function to perform the firmware update
void updateFirmware(const String& firmwareUrl, const String& firmwareTitle, const String& firmwareVersion) {
  Serial.println("Starting firmware update...");
  Serial.println("Update url: " + firmwareUrl);

  client.get(firmwareUrl.c_str());
  int httpResponseCode = client.responseStatusCode();
  String payload = client.responseBody();
  Serial.println("HTTP Response Code: " + String(httpResponseCode));
  Serial.println("HTTP Response Body: " + payload);

  if (httpResponseCode == 200) {
    sendFirmwareState("DOWNLOADED");

    if (Update.begin(UPDATE_SIZE_UNKNOWN)) {
      size_t written = Update.writeStream(client);
      Serial.println("Stream size: " + String(client.contentLength()));
      Serial.println("Written: " + String(written) + " successfully");

      if (written == client.contentLength()) {
        if (Update.end()) {
          if (Update.isFinished()) {
            sendFirmwareState("VERIFIED");
            Serial.println("Update successfully completed. Rebooting.");
            sendFirmwareState("UPDATING");
            ESP.restart();
          } else {
            sendFirmwareState("FAILED", "Update not finished, something went wrong!");
            Serial.println("Update not finished? Something went wrong!");
          }
        } else {
          String errorMsg = "Error #: " + String(Update.getError());
          sendFirmwareState("FAILED", errorMsg.c_str());
          Serial.println("Error Occurred. " + errorMsg);
        }
      } else {
        String errorMsg = "Written only: " + String(written) + "/" + String(client.contentLength()) + ". Retry?";
        sendFirmwareState("FAILED", errorMsg.c_str());
        Serial.println(errorMsg);
      }
    } else {
      String errorMsg = "Not enough space to begin OTA";
      sendFirmwareState("FAILED", errorMsg.c_str());
      Serial.println(errorMsg);
    }
  } else {
    String errorMsg = "Firmware download failed, HTTP response code: " + String(httpResponseCode);
    sendFirmwareState("FAILED", errorMsg.c_str());
    Serial.println(errorMsg);
  }
}

// Function to check for new firmware and perform OTA update
void checkFwVersion() {
  // Ensure WiFi is connected before proceeding
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Waiting for WiFi connection...");
  }

  // Debug: Print WiFi status
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Get firmware details
  String getUrl = String("http://") + serverAddress + ":" + String(serverPort) + "/api/v1/" + token + "/attributes?sharedKeys=fw_checksum,fw_checksum_algorithm,fw_size,fw_tag,fw_title,fw_version";
  Serial.println("GET URL: " + getUrl);

  client.beginRequest();
  client.get(getUrl.c_str());
  int httpResponseCode = client.responseStatusCode();

  Serial.println("HTTP Response Code: " + String(httpResponseCode));

  if (httpResponseCode > 0) {
    String payload = client.responseBody();
    Serial.println("Response Payload: " + payload);

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonObject shared = doc["shared"];
    String latestFirmwareVersion = shared["fw_version"].as<String>();
    String firmwareTitle = shared["fw_title"].as<String>();
    String firmwareTag = shared["fw_tag"].as<String>();
    String firmwareUrl = String("http://") + serverAddress + ":" + String(serverPort) + "/api/v1/" + token + "/firmware?title=" + firmwareTitle + "&version=" + latestFirmwareVersion;

    String currentFirmwareVersion = "1.0.0";  // Replace with the current firmware version of your device
    if (firmwareTag == "FORCE" || latestFirmwareVersion != currentFirmwareVersion) {
      sendFirmwareState("DOWNLOADING");
      updateFirmware(firmwareUrl, firmwareTitle, latestFirmwareVersion);
    }
  } else {
    String errorMsg = "Error on sending GET: " + String(httpResponseCode);
    sendFirmwareState("FAILED", errorMsg.c_str());
    Serial.println(errorMsg);
  }
}

#endif // OTA_CHECK_H
