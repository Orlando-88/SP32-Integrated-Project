#ifndef SG90_H
#define SG90_H

#include <Arduino.h>

#define SG90_PULSE_MIN   500
#define SG90_PULSE_NEUTRAL 3000
#define SG90_PULSE_MAX   4500

void sg90Init(int pin);
void sg90Write(int angle);
void sg90Speed(int speed);
void sg90Stop();
void sg90ProcessKey(char key);

#endif
