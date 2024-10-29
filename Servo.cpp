#include "Servo.h"

Servo::Servo (uint8_t pin, uint8_t channel, uint8_t inverted) {
  this->pin = pin;
  this->channel = channel;

  // Determine polarity
  if (inverted) {
    this->startPos = SERVO_PULSE_WIDTH_MIN;
    this->endPos = SERVO_PULSE_WIDTH_MAX;
  } else {
    this->startPos = SERVO_PULSE_WIDTH_MAX;
    this->endPos = SERVO_PULSE_WIDTH_MIN;
  }

  ledcSetup(channel, 50, SERVO_MAX_BIT_NUM);
  ledcAttachPin(pin, channel);
}

void Servo::setPulseWidth (int width) {
  ledcWrite(channel, width);
}

void Servo::setPos (int pos) {
  currentPos = pos;
  int pulseWidth = map(pos, 0, 1000, startPos, endPos);
  setPulseWidth(pulseWidth);
}

void Servo::moveStart () {
  currentPos = 0;
  setPulseWidth(startPos);
}

void Servo::moveEnd () {
  currentPos = 1000;
  setPulseWidth(endPos);
}