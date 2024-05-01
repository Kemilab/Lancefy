/*400 Bad Request - Invalid URL, request parameters or body.
401 Unauthorized - Invalid $ACCESS_TOKEN.
404 Not Found - Resource not found.
pin 26 - uv sensor adc
*/

/************************************************************************
  LIBRARIES
************************************************************************/
#include <BME280.h>
#include <BME280I2C.h>
#include <BME280I2C_BRZO.h>
#include <EnvironmentCalculations.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "send_data.h"
/************************************************************************
  MACROS
************************************************************************/
#define ENABLE_DEBUG true  // serial debuging
//#define BME_POWER_PWR_PIN 12         // digital pin 12
//#define UV_SENSOR_PWR_PIN 13         // digital pin 13
#define SEALEVELPRESSURE_HPA (1011)  // sea level pressure for altitude
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 5
/**********************************************************************
  VARS
***********************************************************************/

BME280I2C::Settings settings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_16,
  BME280::SpiEnable_False,
  BME280I2C::I2CAddr_0x76);

BME280I2C bme(settings);

int uvIndex = 0;
float uv_Value = 0.0;
float aCoefficient = 17.625;  //Magnus coefficient a for calculating the dew poin
float bCoefficient = 243.04;  //Magnus coefficient b for calculating the dew poin

void setup() {
  if (ENABLE_DEBUG) {
    Serial.begin(115200);
  }
  Wire.begin();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  start_wifi();
  read_bme();
  delay(1000);
  sendData();
  // read_UV();
    Serial.println("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
}
void loop() {}

float read_bme() {
  while (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  SensorData data;

  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  Serial.print(F("Temp: "));
  Serial.print(temp);
  Serial.print(("°" + String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F")));
  Serial.println("\t\tHumidity: ");
  Serial.print(hum);
  Serial.print("% RH");
  Serial.print("\t\tPressure: ");
  Serial.print(pres);
  Serial.print(String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa"));

  EnvironmentCalculations::AltitudeUnit envAltUnit = EnvironmentCalculations::AltitudeUnit_Meters;
  EnvironmentCalculations::TempUnit envTempUnit = EnvironmentCalculations::TempUnit_Celsius;

  float absHum = EnvironmentCalculations::AbsoluteHumidity(temp, hum, envTempUnit);
  float dewPoint = EnvironmentCalculations::DewPoint(temp, hum, envTempUnit);
  //float seaLevel = EnvironmentCalculations::EquivalentSeaLevelPressure(barometerAltitude, temp, pres, envAltUnit, envTempUnit);

  /*Serial.println("\t\tAltitude: ");
  Serial.println(altitude);*/
  //Serial.print((envAltUnit == EnvironmentCalculations::AltitudeUnit_Meters ? "m" : "ft"));
  Serial.println("\t\tDew point: ");
  Serial.print(dewPoint);
  Serial.print("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));
  /*Serial.println("\t\tEquivalent Sea Level Pressure: ");
  Serial.println(seaLevel);*/
  Serial.print("\t\tHeat Index: ");
  float heatIndex = EnvironmentCalculations::HeatIndex(temp, hum, envTempUnit);
  Serial.println(heatIndex);
  Serial.print("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));
  Serial.println("\t\tAbsolute Humidity: ");
  Serial.print(absHum);

  data.temperature = temp;
  data.pressure = pres;
  data.humidity = hum;
  data.dewPoint = dewPoint;
  data.heatIndex = heatIndex;
  data.absHum = absHum;
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

float calculateDewPoin() {
}


void wind_speed() {
  //todo
}