#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

void protocolInit();
void protocolProcess();
void protocolProcessKey(char key);
byte calculateChecksum(const char* str);
void executeCommand(const char* command);

void mode1Start();
void mode2Start();
void mode3Start();
void mode4Start();
void mode5Start();
void modeStop();

#endif
