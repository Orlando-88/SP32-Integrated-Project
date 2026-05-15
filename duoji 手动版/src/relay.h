#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

void relayInit(int pin);
void relayToggle();
void relaySet(bool on);
bool relayGetState();
void relayProcessKey(char key);

#endif
