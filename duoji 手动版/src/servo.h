#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

void servoInit(int pin);
void servoWrite(int angle);
void servoProcessKey(char key);

#endif