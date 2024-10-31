#include <FastLED.h>

#include "Actuator.h"
#include "Jaw.h"
#include "ServoDS3218.h"
#include "MicroServoSG90.h"

#define LEFT_SERVO_CHANNEL 0
#define LEFT_SERVO_PIN GPIO_NUM_44
#define RIGHT_SERVO_CHANNEL 1
#define RIGHT_SERVO_PIN GPIO_NUM_43

#define JAW_SERVO_PIN GPIO_NUM_6
#define JAW_SERVO_CHANNEL 2
#define JAW_SERVO_INVERTED 0


#define EYE_LED_COUNT 21

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

char receivedChars[MAX_CMD_SIZE];

void setLedSpan (int from, int width, CRGB color) {
  for (int i = from; i < from + width; i++) {
    leds[i] = color;
  }
}

void resetEyes () {
  fill_solid(leds, LED_NUM, CRGB(0));
  setLedSpan(LED_MAP_EYE1_INNER_START, LED_MAP_EYE1_INNER_WID, CRGB(0x00ff00));
}

void dilate () {
  setLedSpan(LED_MAP_EYE1_OUTER_START, LED_MAP_EYE1_OUTER_WID, CRGB(0x00ff00));
  setLedSpan(LED_MAP_EYE1_INNER_START, LED_MAP_EYE1_INNER_WID, CRGB(0));
}

void setEyesGreen () {
  fill_solid(leds, LED_NUM, CRGB(0, 255, 0));
}

void setEyesRed () {
  fill_solid(leds, LED_NUM, CRGB(255, 0, 0));
}

void setEyes (int color) {
  fill_solid(leds, LED_NUM, CRGB((uint32_t)color));
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
  while (received != '\n') {
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
  else if (strcmp(buffer, "GREEN") == 0) {
    setEyesGreen();
  } else if (strcmp(buffer, "RED") == 0) {
    setEyesRed();
  } else if (sscanf(buffer, "EYES#%6x", &arg1) == 1) {
    setEyes(arg1);
  } else if (sscanf(buffer, "BRIGHT%d", &arg1) == 1) {
    FastLED.setBrightness(arg1);
  } else if (sscanf(buffer, "JAW%d", &arg1) == 1) {
    jawServo.setPulseWidth(arg1);
    Serial.print("Set jaw to width ");
    Serial.println(arg1);
  } else if (strcmp(buffer, "RSTEYES") == 0) {
    resetEyes();
  } else if (strcmp(buffer, "DILATE") == 0) {
    dilate();
  }
}

void setup() {
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_COLOR_ORDER>(leds, LED_NUM)
    .setCorrection(TypicalLEDStrip)
    .setDither(LED_BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(LED_BRIGHTNESS);

  resetEyes();
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
  FastLED.show();
}
