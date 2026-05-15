#include "protocol.h"
#include "servo.h"
#include "sg90.h"
#include "sg90_180.h"
#include "stepper.h"
#include "relay.h"
#include "relay2.h"
#include <Arduino.h>

const char MSG_START = '<';
const char MSG_END = '>';
const char CHECKSUM_SEP = '*';
const int BUFFER_SIZE = 128;

static char receiveBuffer[BUFFER_SIZE];
static int bufferIndex = 0;
static bool inMessage = false;

byte calculateChecksum(const char* str) {
  byte checksum = 0;
  while (*str) {
    checksum ^= *str++;
  }
  return checksum;
}

void processStepperKey(char c) {
  const int STEPPER_SPEED = 3000;
  switch (c) {
    case 'a': case 'A':
      stepperStartContinuous(STEPPER_SPEED);
      Serial.println("Stepper: Forward (CW)");
      break;
    case 'd': case 'D':
      stepperStartContinuous(-STEPPER_SPEED);
      Serial.println("Stepper: Reverse (CCW)");
      break;
    case 's': case 'S':
      stepperStop();
      Serial.println("Stepper: Stopped");
      break;
    case 'w': case 'W':
      stepperStartContinuous(STEPPER_SPEED * 2);
      Serial.println("Stepper: Fast Forward");
      break;
    case 'e': case 'E':
      stepperStartContinuous(-STEPPER_SPEED * 2);
      Serial.println("Stepper: Fast Reverse");
      break;
  }
}

void processAsKey(char c) {
  servoProcessKey(c);
  sg90ProcessKey(c);
  sg90_180ProcessKey(c);
  processStepperKey(c);
  relayProcessKey(c);
  relay2ProcessKey(c);
  protocolProcessKey(c);
}

void processCommand(char* msg) {
  char* checksumPtr = strchr(msg, CHECKSUM_SEP);
  if (checksumPtr == NULL) {
    Serial.println("ERROR: No checksum separator");
    return;
  }

  *checksumPtr = '\0';
  char command[BUFFER_SIZE];
  strcpy(command, msg);

  char receivedChecksumStr[3];
  strncpy(receivedChecksumStr, checksumPtr + 1, 2);
  receivedChecksumStr[2] = '\0';

  byte expectedChecksum = calculateChecksum(command);
  byte receivedChecksum = strtol(receivedChecksumStr, NULL, 16);

  if (expectedChecksum != receivedChecksum) {
    Serial.printf("ERROR: Checksum mismatch (expected: %02X, got: %s)\n",
                  expectedChecksum, receivedChecksumStr);
    return;
  }

  Serial.printf("OK: Command received: %s\n", command);
  executeCommand(command);
}

void protocolInit() {
  bufferIndex = 0;
  inMessage = false;
}

void protocolProcess() {
  while (Serial.available() > 0) {
    char incoming = Serial.read();

    if (incoming == MSG_START) {
      bufferIndex = 0;
      inMessage = true;
      continue;
    }

    if (inMessage) {
      if (incoming == '\n' || incoming == MSG_END) {
        receiveBuffer[bufferIndex] = '\0';
        if (bufferIndex > 0) {
          processCommand(receiveBuffer);
        }
        inMessage = false;
        continue;
      }

      if (bufferIndex < BUFFER_SIZE - 1) {
        receiveBuffer[bufferIndex++] = incoming;
      }
    } else {
      processAsKey(incoming);
    }
  }
}

void protocolProcessKey(char key) {
  switch (key) {
    case '1': mode1Start(); break;
    case '2': mode2Start(); break;
    case '3': mode3Start(); break;
    case '4': mode4Start(); break;
    case '5': mode5Start(); break;
    case '0': modeStop(); break;
  }
}

void executeCommand(const char* command) {
  if (strcmp(command, "MODE1:START") == 0) {
    mode1Start();
  } else if (strcmp(command, "MODE2:START") == 0) {
    mode2Start();
  } else if (strcmp(command, "MODE3:START") == 0) {
    mode3Start();
  } else if (strcmp(command, "MODE4:START") == 0) {
    mode4Start();
  } else if (strcmp(command, "MODE5:START") == 0) {
    mode5Start();
  } else if (strcmp(command, "MODE:STOP") == 0) {
    modeStop();
  } else {
    Serial.printf("Unknown command: %s\n", command);
  }
}

void mode1Start() {
  const int STEPS = 60000;
  const int SPEED = 3000;

  Serial.println("Executing Mode 1 - Basic operation");

  servoWrite(0);
  delay(1000);

  stepperStartMove(STEPS, SPEED);
  while (!stepperIsDone()) {
    stepperRun();
  }
  Serial.println("Mode1: Stepper forward done");

  servoWrite(90);
  delay(1000);

  stepperStartMove(-STEPS, SPEED);
  while (!stepperIsDone()) {
    stepperRun();
  }
  Serial.println("Mode1: Stepper reverse done, sequence complete");
}

void mode2Start() {
  Serial.println("Executing Mode 2 - Advanced operation");

  sg90_180Write(90);
  delay(200);
  sg90Speed(-180);
  delay(1000);

  sg90Stop();
  relaySet(true);

  sg90Speed(180);
  delay(1000);

  sg90_180Write(180);
  delay(200);
  sg90Stop();
  delay(100);
  sg90Speed(-180);
  delay(2100);

  sg90Stop();
  delay(1500);

  sg90Speed(180);
  delay(2100);

  sg90Stop();

  relay2Set(true);
  delay(3000);
  relay2Set(false);

  stepperStartMove(48000, 3000);
  while (!stepperIsDone()) {
    stepperRun();
  }

  relaySet(false);

  stepperStartMove(-48000, 3000);
  while (!stepperIsDone()) {
    stepperRun();
  }

  sg90Stop();

  Serial.println("Mode2: sequence complete");
}

void mode3Start() {
  Serial.println("Executing Mode 3 - Precision control");
}

void mode4Start() {
  Serial.println("Executing Mode 4 - High speed mode");
}

void mode5Start() {
  Serial.println("Executing Mode 5 - Low power mode");
}

void modeStop() {
  stepperStop();
  sg90Stop();
  Serial.println("Mode: Stopped");
}
