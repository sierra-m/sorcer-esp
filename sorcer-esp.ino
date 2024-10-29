#include <FastLED.h>

#include "Actuator.h"
#include "Servo.h"

#define LEFT_SERVO_CHANNEL 0
#define LEFT_SERVO_PIN GPIO_NUM_44
#define RIGHT_SERVO_CHANNEL 1
#define RIGHT_SERVO_PIN GPIO_NUM_43

#define JAW_SERVO_PIN GPIO_NUM_6
#define JAW_SERVO_CHANNEL 2


#define LED_DATA_PIN GPIO_NUM_3
#define LED_TYPE    WS2812B
#define LED_COLOR_ORDER GRB
#define LED_NUM         19
#define LED_BRIGHTNESS  10

#define LED_MAP_EYE1_INNER_START 12
#define LED_MAP_EYE1_INNER_WID 7
#define LED_MAP_EYE1_OUTER_START 0
#define LED_MAP_EYE1_OUTER_WID 12


#define MAX_CMD_SIZE 20
#define CMD_TIMEOUT_US 500

Actuator leftActuator(LEFT_SERVO_PIN, LEFT_SERVO_CHANNEL, CLOCKWISE); // move clockwise to extend arm up
Actuator rightActuator(RIGHT_SERVO_PIN, RIGHT_SERVO_CHANNEL, COUNTER_CLOCKWISE); // move counter-clockwise to extend arm up

Servo jawServo(JAW_SERVO_PIN, JAW_SERVO_CHANNEL, 0);

CRGB leds[LED_NUM];

char receivedChars[8];

// full extend unloads the assembly
void unload () {
  leftActuator.extend();
  rightActuator.extend();
}

void retract () {
  leftActuator.retract();
  rightActuator.retract();
}

void extend () {
  leftActuator.setPos(950);
  rightActuator.setPos(950);
}

void extendHalf () {
  leftActuator.setPos(500);
  rightActuator.setPos(500);
}

void tiltRight () {
  leftActuator.setPos(950);
  rightActuator.retract();
}

void tiltLeft () {
  leftActuator.retract();
  rightActuator.setPos(950);
}

void bounce () {
  leftActuator.extend(100);
  rightActuator.extend(100);
  delay(150);
  leftActuator.retract(100);
  rightActuator.retract(100);
  delay(150);
}

void openMouth () {
  jawServo.setPos(400);
}

void closeMouth () {
  jawServo.setPos(600);
}

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

void shake (int count = 1) {
  for (int i = 0; i < count; i++) {
    leftActuator.setPos(600);
    rightActuator.setPos(400);
    delay(200);
    leftActuator.setPos(400);
    rightActuator.setPos(600);
    delay(200);
  }
  extendHalf();
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
    extend();
  } else if (strcmp(buffer, "DOWN") == 0) {
    retract();
  } else if (strcmp(buffer, "MID") == 0) {
    extendHalf();
  } else if (strcmp(buffer, "UNLOAD") == 0) {
    unload();
  } else if (strcmp(buffer, "TILTL") == 0) {
    tiltLeft();
  } else if (strcmp(buffer, "TILTR") == 0) {
    tiltRight();
  } else if (strcmp(buffer, "BOUNCE") == 0) {
    bounce();
  } else if (strcmp(buffer, "SHAKE") == 0) {
    shake(2);
  // Jaw motion commands
  } else if (strcmp(buffer, "OPEN") == 0) {
    openMouth();
  } else if (strcmp(buffer, "CLOSE") == 0) {
    closeMouth();
  // Eye commands
  } else if (strcmp(buffer, "GREEN") == 0) {
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
  extendHalf();
  closeMouth();

  Serial.begin(115200);
  Serial.println("Ready! (=^-^=)");
}

void loop() {
  if (Serial.available()) {
    if (receiveMessage(receivedChars)) {
      Serial.print("ACK: ");
      Serial.println(receivedChars);
      handleMessage(receivedChars);
    }
  }
  FastLED.show();
}
