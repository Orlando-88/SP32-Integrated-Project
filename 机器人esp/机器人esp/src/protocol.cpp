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

  // Fine control commands
  } else if (strcmp(command, "ROD:ROTATE") == 0) {
    sg90Write(30);
    Serial.println("ROD:ROTATE: 旋转吸杆 (30 deg)");
  } else if (strcmp(command, "ROD:RETURN") == 0) {
    sg90Write(90);
    Serial.println("ROD:RETURN: 旋回吸杆 (90 deg)");
  } else if (strcmp(command, "ROD:DROP") == 0) {
    sg90Write(150);
    Serial.println("ROD:DROP: 吸杆下降 (150 deg)");
  } else if (strcmp(command, "MOVE:MERGE") == 0) {
    sg90_180Write(0);
    Serial.println("MOVE:MERGE: 移动合并吸杆 (0 deg)");
  } else if (strcmp(command, "MOVE:STOP") == 0) {
    sg90_180Write(90);
    Serial.println("MOVE:STOP: 停止移动吸杆 (90 deg)");
  } else if (strcmp(command, "MOVE:SEPARATE") == 0) {
    sg90_180Write(180);
    Serial.println("MOVE:SEPARATE: 移动拉开吸杆 (180 deg)");
  } else if (strcmp(command, "CLAMP:OPEN") == 0) {
    servoWrite(0);
    Serial.println("CLAMP:OPEN: 打开夹子 (0 deg)");
  } else if (strcmp(command, "CLAMP:CLOSE") == 0) {
    servoWrite(93);
    Serial.println("CLAMP:CLOSE: 关闭夹子 (93 deg)");
  } else if (strcmp(command, "PLATFORM:UP") == 0) {
    stepperStartContinuous(6000);
    Serial.println("PLATFORM:UP: 上移平台");
  } else if (strcmp(command, "PLATFORM:DOWN") == 0) {
    stepperStartContinuous(-6000);
    Serial.println("PLATFORM:DOWN: 下降平台");
  } else if (strcmp(command, "PLATFORM:STOP") == 0) {
    stepperStop();
    Serial.println("PLATFORM:STOP: 停止平台");
  } else if (strcmp(command, "SUCTION:ON") == 0) {
    relaySet(true);
    Serial.println("SUCTION:ON: 打开吸盘");
  } else if (strcmp(command, "SUCTION:OFF") == 0) {
    relaySet(false);
    Serial.println("SUCTION:OFF: 关闭吸盘");
  } else if (strcmp(command, "BLOWER:ON") == 0) {
    relay2Set(true);
    Serial.println("BLOWER:ON: 打开鼓风机");
  } else if (strcmp(command, "BLOWER:OFF") == 0) {
    relay2Set(false);
    Serial.println("BLOWER:OFF: 关闭鼓风机");

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
  sg90Write(0);
  delay(1000);

  sg90Stop();
  relaySet(true);

  sg90Write(180);
  delay(1000);

  sg90_180Write(180);
  delay(200);
  sg90Stop();
  delay(100);
  sg90Write(0);
  delay(2100);

  sg90Stop();
  delay(1500);

  sg90Write(180);
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
