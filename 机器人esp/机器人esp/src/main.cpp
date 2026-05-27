#include <Arduino.h>
#include "stepper.h"
#include "servo.h"
#include "sg90.h"
#include "sg90_180.h"
#include "relay.h"
#include "relay2.h"
#include "protocol.h"
#include "pwm.h"

void setup() {
  Serial.begin(115200);

  stepperInit();
  pwmInit();
  servoInit(47);
  sg90Init(3);
  sg90_180Init(42);
  relayInit(8);
  relay2Init(9);

  delay(500);
  sg90Stop();
  sg90_180Write(90);

  Serial.println("=== Controls ===");
  Serial.println("A - Stepper Forward (CW)");
  Serial.println("D - Stepper Reverse (CCW)");
  Serial.println("S - Stepper Stop");
  Serial.println("W - Stepper Fast Forward");
  Serial.println("E - Stepper Fast Reverse");
  Serial.println("Z - Servo (GPIO47) -> 0 deg");
  Serial.println("X - Servo (GPIO47) -> 90 deg");
  Serial.println("C - Servo (GPIO47) -> 180 deg");
  Serial.println("T - SG90 (GPIO3) -> CW(-180)");
  Serial.println("Y - SG90 (GPIO3) -> Stop");
  Serial.println("U - SG90 (GPIO3) -> CCW(+180)");
  Serial.println("Q - SG90 180 (GPIO42) -> 0 deg");
  Serial.println("R - SG90 180 (GPIO42) -> 90 deg");
  Serial.println("F - SG90 180 (GPIO42) -> 180 deg");
  Serial.println("G - Relay1 (GPIO8) Toggle");
  Serial.println("H - Relay2 (GPIO9) Toggle");
  Serial.println("1-5 - Run Mode 1-5");
  Serial.println("0 - Stop Mode");
  Serial.println("================");
}

void loop() {
  protocolProcess();
  stepperRun();
}
