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

// Pin used for reading button state
#define BUTTON_READ_PIN GPIO_NUM_7
// Pin used for enabling button LED & switch
#define BUTTON_EN_PIN GPIO_NUM_5
// Time offset used for button debouncing
#define BUTTON_DEBOUNCE_MS 100

#define LED_DATA_PIN GPIO_NUM_3
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB

// Jaw LEDs start at the end of the eyes
#define JAW_LED_COUNT 12
#define JAW_LED_START EYES_LED_COUNT

// Number of LEDs is defined by the total of both eyes and the mouth (TBA)
#define LED_NUM (EYES_LED_COUNT + JAW_LED_COUNT)
#define LED_BRIGHTNESS 10

#define MAX_CMD_SIZE 20
#define CMD_TIMEOUT_US 500

typedef enum {
  BUTTON_RELEASED,
  BUTTON_CHANGING,
  BUTTON_PRESSED
} ButtonState;

typedef struct {
  uint8_t valid;
  char cmd;
  char subcmds[2][5];
  uint8_t subcmdsSize;
  char args[3][10];
  uint8_t argsSize;
} CommandDesc;

ServoDS3218 leftArmServo(LEFT_SERVO_PIN, LEFT_SERVO_CHANNEL); // move clockwise to extend arm up
ServoDS3218 rightArmServo(RIGHT_SERVO_PIN, RIGHT_SERVO_CHANNEL); // move counter-clockwise to extend arm up

Actuator actuator(&leftArmServo, &rightArmServo);

MicroServoSG90 jawServo(JAW_SERVO_PIN, JAW_SERVO_CHANNEL);

CRGB leds[LED_NUM];

Eye rightEye(leds, 0, CRGB::Green);
Eye leftEye(leds, EYE_LED_COUNT, CRGB::Green);

Eyes eyes(&leftEye, &rightEye);

Jaw jaw(&jawServo, leds + JAW_LED_START, JAW_LED_COUNT, CRGB::Green);

char receivedChars[MAX_CMD_SIZE];
CommandDesc commandDesc;

ButtonState buttonState;
unsigned long lastButtonTimeMillis;

void reset () {
  eyes.reset();
  jaw.reset();
  FastLED.show();
  actuator.reset();
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

void parseCommand (char * command) {
  // Invalidate previous data
  commandDesc.valid = 0;
  commandDesc.subcmdsSize = 0;
  commandDesc.argsSize = 0;
  if (command[0] == '\0') {
    return;
  }
  commandDesc.cmd = command[0];
  commandDesc.valid = 1;
  if (command[1] != '/') {
    return;
  }
  int index;

  int subcmdIdx = 0;
  int subcmdCharIdx = 0;
  // Move index past first '/'
  for (index = 2; index < MAX_CMD_SIZE && subcmdIdx < 2; index++) {
    switch (command[index]) {
      case '\0':
        break;
      case '/':
        // Null terminate last subcmd stored
        commandDesc.subcmds[subcmdIdx][subcmdCharIdx] = '\0';
        subcmdIdx++;
        subcmdCharIdx = 0;
        continue;
      case '>':
        // Arguments starting
        break;
      default:
        commandDesc.subcmds[subcmdIdx][subcmdCharIdx] = command[index];
        subcmdCharIdx++;
        // No subcmds are > 3 chars - invalid
        if (subcmdCharIdx >= 4) {
          commandDesc.valid = 0;
          commandDesc.subcmds[subcmdIdx][subcmdCharIdx] = '\0';
          return;
        }
        continue;
    }
    break;
  }
  commandDesc.subcmds[subcmdIdx][subcmdCharIdx] = '\0';
  commandDesc.subcmdsSize = subcmdIdx + 1;
  if (index >= MAX_CMD_SIZE || command[index] != '>') {
    return;
  }
  // Increment index to move past '>'
  index++;
  int argIdx = 0;
  int argCharIdx = 0;
  for (; index < MAX_CMD_SIZE && argIdx < 3; index++) {
    switch (command[index]) {
      case '\0':
        break;
      case ',':
        commandDesc.args[argIdx][argCharIdx] = '\0';
        argIdx++;
        argCharIdx = 0;
        continue;
      default:
        commandDesc.args[argIdx][argCharIdx] = command[index];
        argCharIdx++;
        if (argCharIdx >= 9) {
          commandDesc.valid = 0;
          commandDesc.args[argIdx][argCharIdx] = '\0';
        }
        continue;
    }
    break;
  }
  commandDesc.args[argIdx][argCharIdx] = '\0';
  commandDesc.argsSize = argIdx + 1;
}

void handleActuatorCmd (char * command) {
  // Actuator commands take the forms:  A/CMD[>B]
  //                                    A/CMD[>[num][,B]]
  //                                    A/CMD[>[num]]
  //                                      ^
  //                            command starts here
  int arg0;
  if (strncmp(command, "TL", 2) == 0) {
    // Handle:
    // A/TLL[>[num][,B]]
    // A/TLR[>[num][,B]]
    char side, blocking;
    int tiltAmount;
    int numScanned = sscanf(command + 2, "%c>%d,%c", &side, &tiltAmount, &blocking);
    if (numScanned > 0) {
      int chosenTilt = 1000;
      uint8_t chosenBlocking = 0;
      if (numScanned == 2) {
        chosenTilt = constrain(tiltAmount, 0, 1000);
      }
      if (numScanned == 3) {
        chosenBlocking = (blocking == 'B');
      }
      if (side == 'L') {
        actuator.tiltLeft(chosenTilt, chosenBlocking);
      } else if (side == 'R') {
        actuator.tiltRight(chosenTilt, chosenBlocking);
      }
    }
  } else {
    uint8_t blocking = (strncmp(command + 3, ">B", 2) == 0);
    if (strncmp(command, "RST", 3) == 0) {
      actuator.reset();
    } else if (strncmp(command, "UPP", 3) == 0) {
      actuator.extend(blocking);
    } else if (strncmp(command, "DWN", 3) == 0) {
      actuator.retract(blocking);
    } else if (strncmp(command, "MID", 3) == 0) {
      actuator.extendHalf(blocking);
    } else if (strncmp(command, "UNL", 3) == 0) {
      actuator.unload(1);
    } else if (strncmp(command, "BNC", 3) == 0) {
      actuator.bounce(1);
    } else if (strncmp(command, "SHK", 6) == 0) {  // shake can have args, so expand scan size
      actuator.shake(2);
    } else if (sscanf(command, "SHK>%d", &arg0) == 1) {
      arg0 = constrain(arg0, 0, 20);
      actuator.shake(arg0);
    } else if (sscanf(command, "SPD>%d", &arg0) == 1) {
      uint8_t speed = (uint8_t)constrain(arg0, 0, 100);
      actuator.setSpeed(speed);
    }
  }
}

void handleJawCmd (char * command) {
  // Jaw commands all take the form:  J/CMD[>B]
  //                                  J/CMD>[num]
  //                                  J/C>#[hex]
  //                                    ^
  //                          command starts here
  uint8_t blocking = (strncmp(command + 3, ">B", 2) == 0);
  int arg0;
  if (strncmp(command, "OPN", 3) == 0) {
    jaw.open(blocking);
  } else if (strncmp(command, "CLS", 3) == 0) {
    jaw.close(blocking);
  } else if (sscanf(command, "SPD>%d", &arg0) == 1) {
    uint8_t speed = (uint8_t)constrain(arg0, 0, 100);
    jawServo.setSpeed(speed);
  } else if (sscanf(command, "C>#%6x", &arg0) == 1) {
    arg0 = constrain(arg0, 0, 0xffffff);
    jaw.setColor(arg0);
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
  if (strncmp(command, "/LOK", 3) == 0) {
    char direction, side;
    int numScanned = sscanf(command + 4, ">%c,%c", &direction, &side);
    // Must have first arg
    if (numScanned > 0) {
      side = ((numScanned == 2) ? side : 'B');
      if (direction == 'U') {
        switch (side) {
          case 'L':
            leftEye.lookUp(1);
            break;
          case 'R':
            rightEye.lookUp(1);
            break;
          default:
            eyes.lookUp(1);
        }
      } else if (direction == 'D') {
        switch (side) {
          case 'L':
            leftEye.lookDown(1);
            break;
          case 'R':
            rightEye.lookDown(1);
            break;
          default:
            eyes.lookDown(1);
        }
      } else if (direction == 'L') {
        switch (side) {
          case 'L':
            leftEye.lookLeft(1);
            break;
          case 'R':
            rightEye.lookLeft(1);
            break;
          default:
            eyes.lookLeft(1);
        }
      } else if (direction == 'R') {
        switch (side) {
          case 'L':
            leftEye.lookRight(1);
            break;
          case 'R':
            rightEye.lookRight(1);
            break;
          default:
            eyes.lookRight(1);
        }
      }
    }
  } else {
    char subcmd[3];
    char arg0;
    int numScanned = sscanf(command, "/%3c>%c", subcmd, &arg0);
    if (numScanned > 0) {
      char side = ((numScanned == 2) ? arg0 : 'B');
      if (strncmp(subcmd, "OPN", 3) == 0) {
        switch (side) {
          case 'L':
            leftEye.open(1);
            break;
          case 'R':
            rightEye.open(1);
            break;
          default:
            eyes.open(1);
        }
      } else if (strncmp(subcmd, "CLS", 3) == 0) {
        switch (side) {
          case 'L':
            leftEye.close(1);
            break;
          case 'R':
            rightEye.close(1);
            break;
          default:
            eyes.close(1);
        }
      } else if (strncmp(subcmd, "DIL", 3) == 0) {
        switch (side) {
          case 'L':
            leftEye.dilate(1);
            break;
          case 'R':
            rightEye.dilate(1);
            break;
          default:
            eyes.dilate(1);
        }
      } else if (strncmp(subcmd, "CTR", 3) == 0) {
        switch (side) {
          case 'L':
            leftEye.contract(1);
            break;
          case 'R':
            rightEye.contract(1);
            break;
          default:
            eyes.contract(1);
        }
      } else if (strncmp(subcmd, "SQT", 3) == 0) {
        switch (side) {
          case 'L':
            leftEye.squint(1);
            break;
          case 'R':
            rightEye.squint(1);
            break;
          default:
            eyes.squint(1);
        }
      } else if ((strncmp(subcmd, "INF", 3) == 0) && (numScanned == 2)) {
        eyes.setInfill(arg0 == 'Y');
      } else if (strncmp(subcmd, "DIE", 3) == 0) {
        eyes.dead(1);
      } else if (strncmp(subcmd, "CNF", 3) == 0) {
        eyes.confused();
      }
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
      char side, direction;
      if (strncmp(command + 2, "BLK", 3) == 0) {
        int numScanned = sscanf(command + 5, ">%d", &arg0);
        if (numScanned > 0) {
          arg0 = constrain(arg0, 1, 1000);
          eyes.blink(arg0);
        } else {
          eyes.blink();
        }
      } else if (strncmp(command + 2, "SP", 2) == 0) {
        char spiralType = command[4];
        // Validate spiral type
        if (spiralType != 'D' && spiralType != 'L') {
          return;
        }
        // Dot means clear behind
        uint8_t clearBehind = (spiralType == 'D');
        int numScanned = sscanf(command + 5, ">%d,%c,%c", &arg0, &direction, &side);
        uint8_t up = 1;
        if (numScanned > 0) {
          arg0 = constrain(arg0, 1, 1000);
          if (numScanned > 1) {
            up = (direction == 'U');
          }
          if (side == 'L') {
            leftEye.spiral(arg0, up, clearBehind);
          } else if (side == 'R') {
            rightEye.spiral(arg0, up, clearBehind);
          } else {
            if (clearBehind) {
              eyes.spiralDot(arg0, up);
            } else {
              eyes.spiralLine(arg0, up);
            }
          }
        } else {
          if (clearBehind) {
            eyes.spiralDot();
          } else {
            eyes.spiralLine();
          }
        }
      } else if (strncmp(command + 2, "WNK", 3) == 0) {
        arg0 = EYE_BLINK_STEP_DELAY_MS;
        int numScanned = sscanf(command + 5, ">%c,%d", &side, &arg0);
        // First arg required
        if (numScanned > 0) {
          arg0 = constrain(arg0, 1, 1000);
          if (side == 'L') {
            leftEye.blink(arg0);
          } else if (side == 'R') {
            rightEye.blink(arg0);
          }
        }
      } else if (strncmp(command + 2, "RNB", 3) == 0) {
        int numScanned = sscanf(command + 5, ">%d,%c", &arg0, &side);
        if (numScanned > 0) {
          arg0 = constrain(arg0, 1, 1000);
          if (side == 'L') {
            leftEye.rainbow(arg0);
          } else if (side == 'R') {
            rightEye.rainbow(arg0);
          } else {
            eyes.rainbow(arg0);
          }
        } else {
          eyes.rainbow();
        }
      }
      break;
  }
}

void handleButtonCmd (char * command) {
  // Buttom commands take the form: B/ENA
  //                                B/DIS
  //                                  ^
  //                         command starts here
  if (strncmp(command, "ENA", 3) == 0) {
    digitalWrite(BUTTON_EN_PIN, HIGH);
  } else if (strncmp(command, "DIS", 3) == 0) {
    digitalWrite(BUTTON_EN_PIN, LOW);
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
    case 'B':
      handleButtonCmd(subcmd);
      break;
  }
  // Update LEDs if not done already
  FastLED.show();
}

// Handles reading, debouncing, and notifications of button
void handleButton () {
  uint8_t reading = digitalRead(BUTTON_READ_PIN);
  // Button is inverted, so convert to correct new state
  ButtonState newState = (reading ? BUTTON_RELEASED : BUTTON_PRESSED);

  if (buttonState == BUTTON_CHANGING) {
    if ((millis() - lastButtonTimeMillis) > BUTTON_DEBOUNCE_MS) {
      buttonState = newState;
      if (buttonState == BUTTON_PRESSED) {
        Serial.print("B/ON\n");
      } else {
        Serial.print("B/OFF\n");
      }
    }
  } else {
    if (newState != buttonState) {
      buttonState = BUTTON_CHANGING;
      lastButtonTimeMillis = millis();
    }
  }
}

void setup() {
  pinMode(BUTTON_READ_PIN, INPUT);
  pinMode(BUTTON_EN_PIN, OUTPUT);
  digitalWrite(BUTTON_EN_PIN, LOW);

  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_COLOR_ORDER>(leds, LED_NUM)
    .setCorrection(TypicalLEDStrip)
    .setDither(LED_BRIGHTNESS < 255);

  // Set master brightness control
  FastLED.setBrightness(LED_BRIGHTNESS);

  reset();

  Serial.begin(115200);
  Serial.print("Ready! (=^-^=)\n");
}

void loop() {
  if (Serial.available()) {
    if (receiveMessage(receivedChars)) {
      Serial.print("ACK: ");
      Serial.print(receivedChars);
      Serial.print("\n");
      handleMessage(receivedChars);
    }
  }
  leftArmServo.update();
  rightArmServo.update();
  jawServo.update();
  handleButton();
  eyes.update();
}
