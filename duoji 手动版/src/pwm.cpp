#include "pwm.h"
#include <Arduino.h>

void pwmInit() {
  ledcSetup(5, 50, 10);
  ledcSetup(6, 50, 11);
  ledcSetup(7, 50, 12);
}
