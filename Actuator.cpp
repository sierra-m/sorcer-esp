#include "Actuator.h"

Actuator::Actuator (uint8_t pin, uint8_t channel, ExtendDirection direction) {
  //writeChannel = func;
  this->pin = pin;
  this->channel = channel;
  this->direction = direction;

  // Determine polarity, high to low indicates clockwise
  if (direction == CLOCKWISE) {
    this->startPos = PULSE_WIDTH_MAX;
    this->endPos = PULSE_WIDTH_MIN;
  } else {
    this->startPos = PULSE_WIDTH_MIN;
    this->endPos = PULSE_WIDTH_MAX;
  }

  ledcSetup(channel, 50, MAX_BIT_NUM);
  ledcAttachPin(pin, channel);
}

void Actuator::setPulseWidth (int width) {
  ledcWrite(channel, width);
}

void Actuator::setPos (int pos) {
  currentPos = pos;
  int pulseWidth = map(pos, 0, 1000, startPos, endPos);
  setPulseWidth(pulseWidth);
}

void Actuator::retract (int pos) {
  if (pos > 0) {
    int newPos = currentPos - pos;
    if (newPos < 0) {
      newPos = 0;
    }
    setPos(newPos);
  } else {
    currentPos = 0;
    setPulseWidth(startPos);
  }
}

void Actuator::extend (int pos) {
  if (pos > 0) {
    int newPos = currentPos + pos;
    if (newPos > 1000) {
      newPos = 1000;
    }
    setPos(newPos);
  } else {
    currentPos = 1000;
    setPulseWidth(endPos);
  }
}
