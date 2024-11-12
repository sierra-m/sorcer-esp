#include "esp32-hal.h"
#include "Servo.h"

Servo::Servo (uint16_t minPulseWidth, uint16_t maxPulseWidth, int fullMoveDelay, uint8_t pin, uint8_t channel, uint8_t inverted) {
  this->pin = pin;
  this->channel = channel;
  this->fullMoveDelay = fullMoveDelay;

  this->speed = 100; // start at max speed

  // `fullMoveDelay` is measured in milliseconds, but here is being assigned to a microseconds
  // value. This is because:
  //   delayPerStepMicros == (delayPerRotationMillis / 1000 steps) * (1000 us/ 1ms)
  this->minIncrDelayMicros = fullMoveDelay;
  this->maxIncrDelayMicros = this->minIncrDelayMicros * SERVO_MAX_DELAY_MULT;

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
  // No movement needed for equal position
  if (pos == currentPos) {
    return;
  }
  // For max speed, assign pulse width immediately
  if (speed == 100) {
    // Map position to pulse width
    int pulseWidth = map(pos, 0, 1000, startPulseWidth, endPulseWidth);
    setPulseWidth(pulseWidth);
    if (blocking) {
      waitMovement(pos);
    }
    currentPos = pos;
    nextPos = pos;
  } else {
    // Otherwise, begin a directed async move
    nextPos = pos;
    if (blocking) {
      // If blocking, wait while updating position
      while (currentPos != nextPos) {
        update();
      }
    }
  }
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

int Servo::getPos () {
  return currentPos;
}

void Servo::setSpeed (uint8_t newSpeed) {
  speed = newSpeed;
  incrementDelay = map(newSpeed, 0, 100, maxIncrDelayMicros, minIncrDelayMicros);
}

uint8_t Servo::getSpeed () {
  return speed;
}

uint8_t Servo::requiresUpdate () {
  return currentPos != nextPos;
}

void Servo::update () {
  // If current position needs to move, check time asynchronously and move one
  // 1/1000th in that direction if the delay is sufficient
  if (currentPos != nextPos) {
    if ((micros() - lastTimeMicros) > incrementDelay) {
      if (currentPos > nextPos) {
        currentPos--;
      } else {
        currentPos++;
      }
      int pulseWidth = map(currentPos, 0, 1000, startPulseWidth, endPulseWidth);
      setPulseWidth(pulseWidth);
      lastTimeMicros = micros();
    }
  }
}

void Servo::waitMovement (int newPos) {
  delay(calcDelay(newPos));
}