#include "stepper.h"
#include <AccelStepper.h>

const int DIR_PIN = 40;
const int PUL_PIN = 39;
const int ENA_PIN = 41;

static AccelStepper stepper(AccelStepper::DRIVER, PUL_PIN, DIR_PIN);
static bool moving = false;
static bool continuousMode = false;
static int continuousSpeed = 0;

void stepperInit() {
  pinMode(ENA_PIN, OUTPUT);
  digitalWrite(ENA_PIN, HIGH);

  stepper.setMaxSpeed(5000);
  stepper.setAcceleration(5000);
}

void stepperStartMove(int steps, int speed) {
  continuousMode = false;
  continuousSpeed = 0;

  digitalWrite(ENA_PIN, LOW);
  delay(5);

  stepper.setMaxSpeed(speed);
  stepper.move(steps);
  moving = true;
}

void stepperStartContinuous(int speed) {
  continuousMode = true;
  continuousSpeed = speed;

  digitalWrite(ENA_PIN, LOW);
  delay(5);

  stepper.setSpeed(speed);
  moving = true;
}

void stepperStop() {
  continuousMode = false;
  continuousSpeed = 0;
  moving = false;

  stepper.stop();
  digitalWrite(ENA_PIN, HIGH);
}

void stepperSetSpeed(int speed) {
  if (!continuousMode || !moving) return;
  continuousSpeed = speed;
  stepper.setSpeed(speed);
}

void stepperRun() {
  if (!moving) return;

  if (continuousMode) {
    if (!stepper.runSpeed()) {
      stepper.runSpeed();
    }
  } else {
    if (stepper.run()) {
    } else {
      digitalWrite(ENA_PIN, HIGH);
      moving = false;
    }
  }
}

bool stepperIsDone() {
  return !moving;
}
