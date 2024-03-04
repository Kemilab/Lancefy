/*400 Bad Request - Invalid URL, request parameters or body.
401 Unauthorized - Invalid $ACCESS_TOKEN.
404 Not Found - Resource not found.
*/

#include <WiFi.h>
#include "secrets.h"
#include "sendData.h"
#include <Adafruit_BME680.h>

const char *ssid = WiFissid;
const char *password = WiFipassword;

Adafruit_BME680 bme;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("WiFi setup started");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi!");
  }
  Serial.println("Connected to the network with IP address: ");
  Serial.print(WiFi.localIP());

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);
}
void loop() {

  // Read sensor data
  float temperature = bme.temperature;
  float humidity = bme.humidity;
  float pressure = bme.pressure / 100.0;              // Convert to hPa
  float gasResistance = bme.gas_resistance / 1000.0;  // Convert to kOhms

  String jsonPayload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + ",\"pressure\":" + String(pressure) + ",\"gasResistance\":" + String(gasResistance) + "}";
  // Send data to dash.esclone.com
  sendData(jsonPayload);
  delay(5000);
}
