#include "sg90.h"
#include <Arduino.h>

static int sgPin = 3;
static int sgChannel = 6;

static void setDutyFromPulse(float pulseUs) {
  int duty = (pulseUs / 20000.0f) * 2047;
  ledcWrite(sgChannel, duty);
}

void sg90Init(int pin) {
  sgPin = pin;
  ledcAttachPin(pin, sgChannel);
  delay(200);
  sg90Stop();
}

void sg90Write(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  float pulseUs;
  if (angle <= 90) {
    pulseUs = SG90_PULSE_MIN + (angle / 90.0f) * (SG90_PULSE_NEUTRAL - SG90_PULSE_MIN);
  } else {
    pulseUs = SG90_PULSE_NEUTRAL + ((angle - 90) / 90.0f) * (SG90_PULSE_MAX - SG90_PULSE_NEUTRAL);
  }
  setDutyFromPulse(pulseUs);
  Serial.print("SG90 (GPIO");
  Serial.print(sgPin);
  Serial.print("): ");
  Serial.print(angle);
  Serial.print(" deg (");
  Serial.print((int)pulseUs);
  Serial.println(" us)");
}

void sg90Speed(int speed) {
  if (speed < -180) speed = -180;
  if (speed > 180) speed = 180;

  float pulseUs;
  if (speed >= 0) {
    pulseUs = SG90_PULSE_NEUTRAL + (speed / 180.0f) * (SG90_PULSE_MAX - SG90_PULSE_NEUTRAL);
  } else {
    pulseUs = SG90_PULSE_NEUTRAL + (speed / 180.0f) * (SG90_PULSE_NEUTRAL - SG90_PULSE_MIN);
  }
  setDutyFromPulse(pulseUs);
  Serial.print("SG90 speed: ");
  Serial.print(speed);
  Serial.print(" (");
  Serial.print((int)pulseUs);
  Serial.println(" us)");
}

void sg90Stop() {
  sg90Speed(0);
}

void sg90ProcessKey(char key) {
  switch (key) {
    case 't':
    case 'T':
      sg90Speed(-180);
      break;
    case 'y':
    case 'Y':
      sg90Stop();
      break;
    case 'u':
    case 'U':
      sg90Speed(180);
      break;
  }
}
