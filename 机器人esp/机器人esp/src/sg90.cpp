#include "sg90.h"
#include <Arduino.h>

static int sgPin = 3;

static void setPulse(int duty) {
  ledcWrite(sgPin, duty);
}

void sg90Init(int pin) {
  sgPin = pin;
  ledcAttach(pin, 50, 12);
}

void sg90Write(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  float pulseUs = 500 + (angle / 180.0) * 2000;
  int duty = (pulseUs / 20000.0) * 4095;

  setPulse(duty);
  Serial.print("Servo (GPIO");
  Serial.print(sgPin);
  Serial.print("): ");
  Serial.print(angle);
  Serial.println(" deg");
}

void sg90Stop() {
  sg90Write(90);
}

void sg90ProcessKey(char key) {
  switch (key) {
    case 't':
    case 'T':
      sg90Write(30);
      break;
    case 'y':
    case 'Y':
      sg90Write(90);
      break;
    case 'u':
    case 'U':
      sg90Write(150);
      break;
  }
}
