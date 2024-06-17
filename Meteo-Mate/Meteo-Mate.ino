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
#include <LTR390.h>
#include "ota_check.h"
/************************************************************************
  MACROS
************************************************************************/
#define ENABLE_DEBUG true  // serial debugging
//#define BME_POWER_PWR_PIN 12         // digital pin 12
//#define UV_SENSOR_PWR_PIN 13         // digital pin 13
#define SEALEVELPRESSURE_HPA (1011)  // sea level pressure for altitude
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 900  //900 sec
#define I2C_ADDRESS_LTR390 0x53
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

LTR390 ltr390(I2C_ADDRESS_LTR390);

void setup() {
  if (ENABLE_DEBUG) {
    Serial.begin(115200);
  }
  Wire.begin();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  start_wifi();
  read_bme();
  delay(1000);
  read_lux();
  read_UV();
  wind_speed();
  sendData();
  // checkFwVersion(); update!!
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {}

int read_bme() {
  while (!bme.begin() || !ltr390.init()) {
    Serial.println("Sensors error!");
    delay(1000);
  }
  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  EnvironmentCalculations::AltitudeUnit envAltUnit = EnvironmentCalculations::AltitudeUnit_Meters;
  EnvironmentCalculations::TempUnit envTempUnit = EnvironmentCalculations::TempUnit_Celsius;

  float absHum = EnvironmentCalculations::AbsoluteHumidity(temp, hum, envTempUnit);
  float dewPoint = EnvironmentCalculations::DewPoint(temp, hum, envTempUnit);
  float heatIndex = EnvironmentCalculations::HeatIndex(temp, hum, envTempUnit);

  data.temperature = temp;
  data.pressure = pres;
  data.humidity = hum;
  data.dewPoint = dewPoint;
  data.heatIndex = heatIndex;
  data.absHum = absHum;

  Serial.print(F("Temp: "));
  Serial.print(temp);
  Serial.print(("°" + String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F")));
  Serial.print("\t\tHumidity: ");
  Serial.print(hum);
  Serial.print("% RH");
  Serial.print("\t\tPressure: ");
  Serial.print(pres);
  Serial.println(String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa"));
  Serial.print("\t\tDew point: ");
  Serial.print(dewPoint);
  Serial.println("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));
  Serial.print("\t\tHeat Index: ");
  Serial.print(heatIndex);
  Serial.println("°" + String(envTempUnit == EnvironmentCalculations::TempUnit_Celsius ? "C" : "F"));
  Serial.print("\t\tAbsolute Humidity: ");
  Serial.println(absHum);
}

void read_lux() {
  ltr390.setMode(LTR390_MODE_ALS);
  ltr390.setGain(LTR390_GAIN_6);                  // Adjust gain if necessary
  ltr390.setResolution(LTR390_RESOLUTION_18BIT);  // Adjust resolution if necessary

  Serial.print("Ambient Light Lux: ");
  float lux = ltr390.getLux();
  Serial.println(lux);
  data.luxLevel = lux;
}

void read_UV() {
  ltr390.setMode(LTR390_MODE_UVS);
  ltr390.setGain(LTR390_GAIN_18);                 // Recommended for UVI - x18
  ltr390.setResolution(LTR390_RESOLUTION_18BIT);  // Recommended for UVI - 20-bit

  Serial.print("UV Index: ");
  float uvIndex = ltr390.getUVI();
  Serial.println(uvIndex);
  data.uvIndex = uvIndex;
}

void wind_speed() {
  const int analogPin = 25;             // Analog pin connected to the anemometer sensor
  const int threshold = 512;            // Threshold value for detecting rotations 
  const unsigned long interval = 1000;  // 1-second interval for frequency calculation

  volatile int pulseCount = 0;  // Count of threshold crossings
  unsigned long lastTime = 0;

  float k = 0.1;                // Calibration constant
  bool aboveThreshold = false;  // Track if the last value was above the threshold

    unsigned long currentTime = millis();
    int sensorValue = analogRead(analogPin);
    if (sensorValue > threshold && !aboveThreshold) {
      pulseCount++;
      aboveThreshold = true;
    } else if (sensorValue <= threshold) {
      aboveThreshold = false;
    }

    if (currentTime - lastTime >= interval) {
      float frequency = pulseCount / (interval / 1000.0);  // Calculate frequency in Hz
      Serial.println(frequency);
      float windSpeed = (k * frequency) * 3.6 ;  // Calculate wind speed in m/s

      Serial.print("Wind Speed: ");
      Serial.print(windSpeed);
      Serial.println(" km/h");
      data.wind_speed = windSpeed;

      pulseCount = 0;  // Reset pulse count
      lastTime = currentTime;
    }
}
