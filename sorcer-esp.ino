#include <FastLED.h>

#include "src/ServoDS3218.h"
#include "src/MicroServoSG90.h"

#include "src/Actuator.h"
#include "src/Jaw.h"

#include "src/Eye.h"
#include "src/Eyes.h"

#define LEFT_SERVO_CHANNEL 0
#define LEFT_SERVO_PIN GPIO_NUM_44
#define RIGHT_SERVO_CHANNEL 1
#define RIGHT_SERVO_PIN GPIO_NUM_43

#define JAW_SERVO_PIN GPIO_NUM_6
#define JAW_SERVO_CHANNEL 2
#define JAW_SERVO_INVERTED 0

#define LED_DATA_PIN GPIO_NUM_3
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB

// Number of LEDs is defined by the total of both eyes and the mouth (TBA)
#define LED_NUM EYES_LED_COUNT
#define LED_BRIGHTNESS  10

#define MAX_CMD_SIZE 20
#define CMD_TIMEOUT_US 500

ServoDS3218 leftArmServo(LEFT_SERVO_PIN, LEFT_SERVO_CHANNEL); // move clockwise to extend arm up
ServoDS3218 rightArmServo(RIGHT_SERVO_PIN, RIGHT_SERVO_CHANNEL); // move counter-clockwise to extend arm up

Actuator actuator(&leftArmServo, &rightArmServo);

MicroServoSG90 jawServo(JAW_SERVO_PIN, JAW_SERVO_CHANNEL);

Jaw jaw(&jawServo);

CRGB leds[LED_NUM];

Eye leftEye(leds, 0, CRGB::Green);
Eye rightEye(leds, EYE_LED_COUNT, CRGB::Green);

Eyes eyes(&leftEye, &rightEye);

char receivedChars[MAX_CMD_SIZE];

void reset () {
  eyes.reset();
  FastLED.show();
}

uint8_t waitSerial () {
  unsigned long start = micros();
  while (!Serial.available()) {
    delayMicroseconds(50);
    if ((micros() - start) > CMD_TIMEOUT_US) return false;
  }
  return true;
}

uint8_t receiveMessage (char * buffer) {
  uint8_t index = 0;
  uint8_t status = true;
  char received = Serial.read();
  while (received != '\n' && index < (MAX_CMD_SIZE - 1)) {
    buffer[index++] = received;
    if (!waitSerial()) {
      status = false;
      break;
    }
    received = Serial.read();
  }
  buffer[index] = '\0';
  return status;
}

void handleActuatorCmd (char * command) {
  // Actuator commands all take the form:  A/CMD
  int arg0;
  if (strncmp(command, "UP", 3) == 0) {
    actuator.extend();
  } else if (strncmp(command, "DWN", 3) == 0) {
    actuator.retract();
  } else if (strncmp(command, "MID", 3) == 0) {
    actuator.extendHalf();
  } else if (strncmp(command, "UNL", 3) == 0) {
    actuator.unload(1);
  } else if (strncmp(command, "TLL", 3) == 0) {
    actuator.tiltLeft();
  } else if (strncmp(command, "TLR", 3) == 0) {
    actuator.tiltRight();
  } else if (strncmp(command, "BNC", 3) == 0) {
    actuator.bounce(1);
  } else if (strncmp(command, "SHK", 6) == 0) {  // shake can have args, so expand scan size
    actuator.shake(2);
  } else if (sscanf(command, "SHK>%d", &arg0) == 1) {
    arg0 = constrain(arg0, 0, 20);
    actuator.shake(arg0);
  }
}

void handleJawCmd (char * command) {
  // Jaw commands all take the form:  J/CMD
  if (strncmp(command, "OPN", 3) == 0) {
    jaw.open();
  } else if (strncmp(command, "CLS", 3) == 0) {
    jaw.close();
  }
}

void setEyeColor (char side, CRGB color) {
  switch (side) {
    case 'L':
      leftEye.setColor(color);
      break;
    case 'R':
      rightEye.setColor(color);
      break;
    default:
      eyes.setColor(color);
  }
}

void handleEyeColorCmd (char * command) {
  // Color setting:  E/C/CMD[>[L/R]]
  //                 E/C>#[num][,[L/R]]
  //                    ^
  //           command starts here
  int color;
  char side;
  // Check for variable color setting pattern
  int numScanned = sscanf(command, ">#%6x,%c", &color, &side);
  // Constrain color either way
  color = constrain(color, 0, 0xffffff);
  if (numScanned == 1) {
    eyes.setColor(color);
  } else if (numScanned == 2) {
    setEyeColor(side, color);
  } else {
    // No match for variable setting - try named colors
    char subcmd[3];
    color = 0;
    numScanned = sscanf(command, "/%3c>%c", subcmd, &side);
    if (numScanned > 0) {
      if (strncmp(subcmd, "GRN", 3) == 0) {
        color = 0x00ff00;
      } else if (strncmp(subcmd, "RED", 3) == 0) {
        color = 0xff0000;
      } else if (strncmp(subcmd, "BLU", 3) == 0) {
        color = 0x0000ff;
      } else if (strncmp(subcmd, "YLW", 3) == 0) {
        color = 0xffff00;
      } else if (strncmp(subcmd, "PRP", 3) == 0) {
        color = 0xff00ff;
      } else if (strncmp(subcmd, "ORG", 3) == 0) {
        color = 0xffaa00;
      }
    }
    if (color > 0) {
      if (numScanned == 1) {
        eyes.setColor(color);
      } else if (numScanned == 2) {
        setEyeColor(side, color);
      }
    }
  }
}

void handleEyeDrawingCmd (char * command) {
  // Drawing:        E/D/CMD[>[arg]]
  //                    ^
  //           command starts here
  char subcmd[3];
  char arg0;
  int numScanned = sscanf(command, "/%3c>%c", subcmd, &arg0);

  if (numScanned > 0) {
    char side = ((numScanned == 2) ? arg0 : 'B');
    if (strncmp(subcmd, "OPN", 3) == 0) {
      switch (side) {
        case 'L':
          leftEye.open();
          break;
        case 'R':
          rightEye.open();
          break;
        default:
          eyes.open();
      }
    } else if (strncmp(subcmd, "CLS", 3) == 0) {
      switch (side) {
        case 'L':
          leftEye.close();
          break;
        case 'R':
          rightEye.close();
          break;
        default:
          eyes.close();
      }
    } else if (strncmp(subcmd, "DIL", 3) == 0) {
      switch (side) {
        case 'L':
          leftEye.dilate();
          break;
        case 'R':
          rightEye.dilate();
          break;
        default:
          eyes.dilate();
      }
    } else if (strncmp(subcmd, "CTR", 3) == 0) {
      switch (side) {
        case 'L':
          leftEye.contract();
          break;
        case 'R':
          rightEye.contract();
          break;
        default:
          eyes.contract();
      }
    } else if (strncmp(subcmd, "SQT", 3) == 0) {
      switch (side) {
        case 'L':
          leftEye.squint();
          break;
        case 'R':
          rightEye.squint();
          break;
        default:
          eyes.squint();
      }
    } else if ((strncmp(subcmd, "INF", 3) == 0) && (numScanned == 2)) {
      eyes.setInfill(arg0 == 'Y');
    } else if (strncmp(subcmd, "DIE", 3) == 0) {
      eyes.dead();
    } else if (strncmp(subcmd, "RNB", 3) == 0) {
      eyes.rainbow();
    } else if (strncmp(subcmd, "CNF", 3) == 0) {
      eyes.confused();
    }
  }
}

void handleEyeCmd (char * command) {
  // Eye commands take the forms:
  // Color setting:  E/C/CMD[>[L or R]]
  //                 E/C>#[num][,[L or R]]
  // Brightness:     E/B>[num]
  // Reset:          E/R
  // Drawing:        E/D/CMD[>[arg]]
  // Animation:      E/A/CMD[>[L/R]]
  //                   ^
  //          command starts here
  char * subcmd = command + 2;
  int arg0;
  switch (command[0]) {
    case 'C':
      handleEyeColorCmd(command + 1);
      break;
    case 'B':
      if (sscanf(command + 1, ">%d", &arg0) == 1) {
        arg0 = constrain(arg0, 0, 255);
        FastLED.setBrightness(arg0);
      }
      break;
    case 'R':
      eyes.reset();
      break;
    case 'D':
      handleEyeDrawingCmd(command + 1);
      break;
    case 'A':
      // Check delimiter
      if (command[1] != '/') {
        return;
      }
      char side;
      if (strncmp(command + 2, "BLK", 3) == 0) {
        eyes.blink();
      } else if (strncmp(command + 2, "SPD", 3) == 0) {
        eyes.spiralDot();
      } else if (strncmp(command + 2, "SPL", 3) == 0) {
        eyes.spiralLine();
      } else if (sscanf(command + 2, "WNK>%c", &side) == 1) {
        if (side == 'L') {
          leftEye.blink();
        } else if (side == 'R') {
          rightEye.blink();
        }
      }
      break;
  }
}

void handleMessage (char * buffer) {
  if (buffer[0] == 'R') {
    reset();
    return;
  }
  // Check first delimiter
  if (buffer[1] != '/') {
    return;
  }
  char * subcmd = buffer + 2; // Subcommand starts 2 chars in
  switch (buffer[0]) {
    case 'A':
      handleActuatorCmd(subcmd);  
      break;
    case 'J':
      handleJawCmd(subcmd);
      break;
    case 'E':
      handleEyeCmd(subcmd);
      break;
  }
  // Update LEDs if not done already
  FastLED.show();
}

void setup() {
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_COLOR_ORDER>(leds, LED_NUM)
    .setCorrection(TypicalLEDStrip)
    .setDither(LED_BRIGHTNESS < 255);

  // Set master brightness control
  FastLED.setBrightness(LED_BRIGHTNESS);

  reset();
  actuator.reset();
  jaw.close(1);

  Serial.begin(115200);
  Serial.println("Ready! (=^-^=)");
}

void loop() {
  if (Serial.available()) {
    if (receiveMessage(receivedChars)) {
      handleMessage(receivedChars);
      Serial.print("ACK: ");
      Serial.println(receivedChars);
    }
  }
}
