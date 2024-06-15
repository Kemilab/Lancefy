const int analogPin = 25;      // Analog pin connected to the anemometer sensor
const int threshold = 512;     // Threshold value for detecting rotations (adjust as needed)
const unsigned long interval = 1000; // 1-second interval for frequency calculation

volatile int pulseCount = 0;   // Count of threshold crossings
unsigned long lastTime = 0;

float k = 0.1; // Calibration constant (example value, you need to determine this)
bool aboveThreshold = false; // Track if the last value was above the threshold

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long currentTime = millis();
  int sensorValue = analogRead(analogPin);
  if (sensorValue > threshold && !aboveThreshold) {
    pulseCount++;
    aboveThreshold = true;
  } else if (sensorValue <= threshold) {
    aboveThreshold = false;
  }

  if (currentTime - lastTime >= interval) {
    float frequency = pulseCount / (interval / 1000.0); // Calculate frequency in Hz
    Serial.println(frequency);
    float windSpeed = k * frequency; // Calculate wind speed in m/s

    Serial.print("Wind Speed: ");
    Serial.print(windSpeed);
    Serial.println(" m/s");

    pulseCount = 0; // Reset pulse count
    lastTime = currentTime;
  }
}
