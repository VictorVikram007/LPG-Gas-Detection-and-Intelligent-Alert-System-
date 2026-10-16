// LPG sensor -> Serial (9600 baud), one JSON line per reading: {"ppm":523}
// Works with MQ-6 (best for LPG) or MQ-2. Arduino Uno: A0. ESP32: use GPIO34 and ADC_MAX 4095.
const int   SENSOR_PIN = A0;
const int   BUZZER_PIN = 8;      // local buzzer so the alarm works even with no PC
const float ADC_MAX    = 1023.0;
const float RL         = 10.0;   // load resistor on your module, kOhm
float       R0         = 10.0;   // calibrated in clean air at startup (see setup)
const int   DANGER_PPM = 1000;

// MQ-6 LPG curve from datasheet: ppm = A * (Rs/R0)^B. Tune A/B for your module.
const float A = 1009.2, B = -2.35;

float readRs() {
  float v = analogRead(SENSOR_PIN) / ADC_MAX;
  if (v < 0.001) v = 0.001;
  return RL * (1.0 - v) / v;
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  // Calibrate R0 in clean air (MQ-6 clean-air Rs/R0 ~= 10). Let the heater warm up first for best results.
  delay(20000);
  float sum = 0;
  for (int i = 0; i < 50; i++) { sum += readRs(); delay(50); }
  R0 = (sum / 50) / 10.0;
}

void loop() {
  float ppm = A * pow(readRs() / R0, B);
  digitalWrite(BUZZER_PIN, ppm >= DANGER_PPM);
  Serial.print("{\"ppm\":");
  Serial.print(ppm, 0);
  Serial.println("}");
  delay(500);
}
