#include "sg90_180.h"
#include <Arduino.h>

static int sgPin = 42;

static void setPulse(int duty) {
  ledcWrite(sgPin, duty);
}

void sg90_180Init(int pin) {
  sgPin = pin;
  ledcAttach(pin, 50, 12);
}

void sg90_180Write(int angle) {
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

void sg90_180ProcessKey(char key) {
  switch (key) {
    case 'q':
    case 'Q':
      sg90_180Write(0);
      break;
    case 'r':
    case 'R':
      sg90_180Write(90);
      break;
    case 'f':
    case 'F':
      sg90_180Write(180);
      break;
  }
}
