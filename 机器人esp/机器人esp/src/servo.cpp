#include "servo.h"
#include <Arduino.h>

static int servoPin = 47;

static void setPulse(int duty) {
  ledcWrite(servoPin, duty);
}

void servoInit(int pin) {
  servoPin = pin;
  ledcAttach(pin, 50, 12);
}

void servoWrite(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;

  float pulseUs = 500 + (angle / 180.0) * 2000;
  int duty = (pulseUs / 20000.0) * 4095;

  setPulse(duty);
  Serial.print("Servo (GPIO");
  Serial.print(servoPin);
  Serial.print("): ");
  Serial.print(angle);
  Serial.println(" deg");
}

void servoProcessKey(char key) {
  switch (key) {
    case 'z':
    case 'Z':
      servoWrite(0);
      break;
    case 'x':
    case 'X':
      servoWrite(93);
      break;
    case 'c':
    case 'C':
      servoWrite(180);
      break;
  }
}
