#include "relay.h"
#include <Arduino.h>

static int relayPin = -1;
static bool relayState = false;

void relayInit(int pin) {
  relayPin = pin;
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  relayState = false;
}

void relayToggle() {
  if (relayPin < 0) return;
  relayState = !relayState;
  digitalWrite(relayPin, relayState ? HIGH : LOW);
  Serial.print("Relay: ");
  Serial.println(relayState ? "ON" : "OFF");
}

void relaySet(bool on) {
  if (relayPin < 0) return;
  relayState = on;
  digitalWrite(relayPin, relayState ? HIGH : LOW);
  Serial.print("Relay: ");
  Serial.println(relayState ? "ON" : "OFF");
}

bool relayGetState() {
  return relayState;
}

void relayProcessKey(char key) {
  switch (key) {
    case 'g':
    case 'G':
      relayToggle();
      break;
  }
}
