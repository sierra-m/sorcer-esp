#include <FastLED.h>

#include "Actuator.h"
#include "Jaw.h"
#include "ServoDS3218.h"
#include "MicroServoSG90.h"

#include "Eye.h"
#include "Eyes.h"

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
#define LED_NUM EYE_LED_COUNT * 2
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

void handleMessage (char * buffer) {
  int arg1;
  // Head motion commands
  if (strcmp(buffer, "UP") == 0) {
    actuator.extend();
  } else if (strcmp(buffer, "DOWN") == 0) {
    actuator.retract();
  } else if (strcmp(buffer, "MID") == 0) {
    actuator.extendHalf();
  } else if (strcmp(buffer, "UNLOAD") == 0) {
    actuator.unload(1);
  } else if (strcmp(buffer, "TILTL") == 0) {
    actuator.tiltLeft();
  } else if (strcmp(buffer, "TILTR") == 0) {
    actuator.tiltRight();
  } else if (strcmp(buffer, "BOUNCE") == 0) {
    actuator.bounce(1);
  } else if (strcmp(buffer, "SHAKE") == 0) {
    actuator.shake(2);
  }
  // Jaw motion commands
  else if (strcmp(buffer, "OPEN") == 0) {
    jaw.open();
  } else if (strcmp(buffer, "CLOSE") == 0) {
    jaw.close();
  }
  // Eye commands
  else if (strcmp(buffer, "HAPPY") == 0) {
    eyes.happy();
  } else if (strcmp(buffer, "ANGRY") == 0) {
    eyes.angry();
  } else if (strcmp(buffer, "CAUTION") == 0) {
    eyes.caution();
  } else if (sscanf(buffer, "EYES#%6x", &arg1) == 1) {
    arg1 = constrain(arg1, 0, 0xffffff);
    eyes.setColor(CRGB(arg1));
  } else if (sscanf(buffer, "BRIGHT%d", &arg1) == 1) {
    arg1 = constrain(arg1, 0, 255);
    FastLED.setBrightness(arg1);
  } else if (strcmp(buffer, "RSTEYES") == 0) {
    eyes.reset();
  } else if (strcmp(buffer, "DILATE") == 0) {
    leftEye.dilate();
    rightEye.dilate();
  } else if (strcmp(buffer, "CONTRACT") == 0) {
    leftEye.contract();
    rightEye.contract();
  } else if (strcmp(buffer, "BLINK") == 0) {
    eyes.blink();
  } else if (strcmp(buffer, "WINKL") == 0) {
    leftEye.blink();
  } else if (strcmp(buffer, "WINKR") == 0) {
    rightEye.blink();
  } else if (strcmp(buffer, "SQUINT") == 0) {
    eyes.squint();
  } else if (strcmp(buffer, "SQUINTL") == 0) {
    leftEye.squint();
  } else if (strcmp(buffer, "SQUINTR") == 0) {
    rightEye.squint();
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

  eyes.reset();
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
