#include "Servo.h"

Servo::Servo (uint16_t minPulseWidth, uint16_t maxPulseWidth, int fullMoveDelay, uint8_t pin, uint8_t channel, uint8_t inverted) {
  this->pin = pin;
  this->channel = channel;
  this->fullMoveDelay = fullMoveDelay;

  // Determine polarity, such that noninverted positive movement (0 -> 1000) is clockwise
  if (inverted) {
    this->startPulseWidth = minPulseWidth;
    this->endPulseWidth = maxPulseWidth;
  } else {
    this->startPulseWidth = maxPulseWidth;
    this->endPulseWidth = minPulseWidth;
  }

  // Set up 50Hz PWM wave for given channel
  ledcSetup(channel, 50, SERVO_MAX_BIT_NUM);
  // Attach to GPIO
  ledcAttachPin(pin, channel);
}

void Servo::setPulseWidth (int width) {
  ledcWrite(channel, width);
}

void Servo::setPos (int pos, uint8_t blocking) {
  int pulseWidth = map(pos, 0, 1000, startPulseWidth, endPulseWidth);
  setPulseWidth(pulseWidth);
  if (blocking) {
    waitMovement(pos);
  }
  currentPos = pos;
}

void Servo::moveStart (uint8_t blocking) {
  setPulseWidth(startPulseWidth);
  if (blocking) {
    waitMovement(0);
  }
  currentPos = 0;
}

void Servo::moveEnd (uint8_t blocking) {
  setPulseWidth(endPulseWidth);
  if (blocking) {
    waitMovement(1000);
  }
  currentPos = 1000;
}

void Servo::invert () {
  int temp = startPulseWidth;
  startPulseWidth = endPulseWidth;
  endPulseWidth = temp;
}

int Servo::calcDelay (int newPos) {
  int delta = abs(newPos - currentPos);
  return (fullMoveDelay * delta) / 1000;
}

void Servo::waitMovement (int newPos) {
  delay(calcDelay(newPos));
}