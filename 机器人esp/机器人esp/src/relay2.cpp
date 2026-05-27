#include "relay2.h"
#include <Arduino.h>

static int relay2Pin = -1;
static bool relay2State = false;

void relay2Init(int pin) {
  relay2Pin = pin;
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay2Pin, LOW);
  relay2State = false;
}

void relay2Toggle() {
  if (relay2Pin < 0) return;
  relay2State = !relay2State;
  digitalWrite(relay2Pin, relay2State ? HIGH : LOW);
  Serial.print("Relay2: ");
  Serial.println(relay2State ? "ON" : "OFF");
}

void relay2Set(bool on) {
  if (relay2Pin < 0) return;
  relay2State = on;
  digitalWrite(relay2Pin, relay2State ? HIGH : LOW);
  Serial.print("Relay2: ");
  Serial.println(relay2State ? "ON" : "OFF");
}

bool relay2GetState() {
  return relay2State;
}

void relay2ProcessKey(char key) {
  switch (key) {
    case 'h':
    case 'H':
      relay2Toggle();
      break;
  }
}
