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

ServoDS3218 leftArmServo(LEFT_SERVO_PIN, LEFT_SERVO_CHANNEL); // move clockwise to extend arm up
ServoDS3218 rightArmServo(RIGHT_SERVO_PIN, RIGHT_SERVO_CHANNEL); // move counter-clockwise to extend arm up

Actuator actuator(&leftArmServo, &rightArmServo);

MicroServoSG90 jawServo(JAW_SERVO_PIN, JAW_SERVO_CHANNEL);

CRGB leds[LED_NUM];

Eye leftEye(leds, 0, CRGB::Green);
Eye rightEye(leds, EYE_LED_COUNT, CRGB::Green);

Eyes eyes(&leftEye, &rightEye);

Jaw jaw(&jawServo, leds + JAW_LED_START, JAW_LED_COUNT, CRGB::Green);

char receivedChars[MAX_CMD_SIZE];

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
}
