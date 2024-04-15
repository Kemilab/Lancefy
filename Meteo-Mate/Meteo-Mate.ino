/*400 Bad Request - Invalid URL, request parameters or body.
401 Unauthorized - Invalid $ACCESS_TOKEN.
404 Not Found - Resource not found.
pin 26 - uv sensor adc
*/
#include "Adafruit_BME680.h"
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "send_data.h"

Adafruit_BME680 bme;
#define SEALEVELPRESSURE_HPA (1011)

int uvIndex = 0;
double uv_Value = 0.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  start_wifi();
  read_bme();
  read_UV();
}

void read_bme() {
  bme.begin();
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);  // 320*C for 150 ms

  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
}

void read_UV() {  // impement that uv value is being stored in uvIndex variable for easier sending
  uv_Value = analogRead(26) * 5000 / 1023.0;
  if (uv_Value > 0 && uv_Value < 227) {
    Serial.println("UV index is 0");
    uvIndex = 0;
  } else if (uv_Value > 50 && uv_Value < 318) {
    Serial.println("UV index is 1");
    uvIndex = 1;
  } else if (uv_Value > 227 && uv_Value < 408) {
    Serial.println("UV index is 2");
    uvIndex = 2;
  } else if (uv_Value > 318 && uv_Value < 503) {
    Serial.println("UV index is 3");
    uvIndex = 3;
  } else if (uv_Value > 408 && uv_Value < 606) {
    Serial.println("UV index is 4");
    uvIndex = 4;
  } else if (uv_Value > 503 && uv_Value < 696) {
    Serial.println("UV index is 5");
    uvIndex = 5;
  } else if (uv_Value > 606 && uv_Value < 795) {
    Serial.println("UV index is 6");
    uvIndex = 6;
  } else if (uv_Value > 696 && uv_Value < 881) {
    Serial.println("UV index is 7");
    uvIndex = 7;
  } else if (uv_Value > 795 && uv_Value < 976) {
    Serial.println("UV index is 8");
    uvIndex = 8;
  } else if (uv_Value > 881 && uv_Value < 1079) {
    Serial.println("UV index is 9");
    uvIndex = 9;
  } else if (uv_Value > 976 && uv_Value < 1170) {
    Serial.println("UV index is 10");
    uvIndex = 10;
  } else if (uv_Value >= 1170) {
    Serial.println("UV index is 11+");
    uvIndex = 11;
  } else {
    Serial.println("Data is out of range or sensor is broken!");
  }
  delay(1000);
  Serial.println("Raw data values for uv is " + (String)uv_Value);
}




void wind_speed() {
  //todo
}