/*400 Bad Request - Invalid URL, request parameters or body.
401 Unauthorized - Invalid $ACCESS_TOKEN.
404 Not Found - Resource not found.
*/

#include <WiFi.h>
#include "secrets.h"
#include <Adafruit_BME680.h>
#include <Wire.h>

const char *ssid = WiFissid;
const char *password = WiFipassword;
const char *key = key;

Adafruit_BME680 bme;

void setup() {
  Serial.begin(115200);
}
void loop() {
  readBME680();
  delay(300);
}

void readBME680() {
  // Read sensor data
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while(1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  float temperature = bme.temperature;
  float humidity = bme.humidity;
  float pressure = bme.pressure / 100.0;              // Convert to hPa
  float gasResistance = bme.gas_resistance / 1000.0;  // Convert to kOhms

  Serial.print("Printing values");
  Serial.println(String("Temperature is ") + String(temperature));
  Serial.println(String("Humidity is ") + String(humidity));
  Serial.println(String("Pressure is ") + String(pressure));
  Serial.println(String("gasResistance is ") + String(gasResistance));
}

void read_UV() {
  //todo
}

void wind_speed() {
  //todo
}