#include "Actuator.h"

Actuator::Actuator (ServoDS3218 *leftServo, ServoDS3218 *rightServo, ExtendDirection leftServoDir) {
  this->leftServo = leftServo;
  this->rightServo = rightServo;

  // Servos should both be initialized to noninverted configuration, ensure one is inverted
  // such that a positive position change is associated with actuator extension
  if (leftServoDir == CLOCKWISE) {
    this->rightServo->invert();
  } else {
    this->leftServo->invert();
  }
}

void Actuator::moveBoth (int leftNewPos, int rightNewPos, uint8_t blocking) {
  leftServo->setPos(leftNewPos);
  rightServo->setPos(rightNewPos);
  if (blocking) {
    waitMaxDelay(leftNewPos, rightNewPos);
  }
}

void Actuator::reset () {
  // Call is always blocking to ensure reset before other actions
  extendHalf(1);
}

void Actuator::retract (uint8_t blocking) {
  leftServo->moveStart();
  rightServo->moveStart();
  if (blocking) {
    waitMaxDelay(0, 0);
  }
}

void Actuator::extend (uint8_t blocking) {
  moveBoth(950, 950, blocking);
}

void Actuator::unload (uint8_t blocking) {
  leftServo->moveEnd();
  rightServo->moveEnd();
  if (blocking) {
    waitMaxDelay(1000, 1000);
  }
}

void Actuator::extendHalf (uint8_t blocking) {
  moveBoth(500, 500, blocking);
}

void Actuator::tiltRight (int amount, uint8_t blocking) {
  int halfAmount = amount / 2;
  moveBoth(500 + halfAmount, 500 - halfAmount, blocking);
}

void Actuator::tiltLeft (int amount, uint8_t blocking) {
  int halfAmount = amount / 2;
  moveBoth(500 - halfAmount, 500 + halfAmount, blocking);
}

void Actuator::bounce (uint8_t blocking) {
  int initialLeftPos = leftServo->getPos();
  int initialRightPos = rightServo->getPos();
  // Initial move must be a blocking call
  moveBoth(initialLeftPos + 100, initialRightPos + 100, 1);
  // Second move may be nonblocking
  moveBoth(initialLeftPos, initialRightPos, blocking);
}

void Actuator::shake (int count) {
  // Start from default position
  reset();
  for (int i = 0; i < count; i++) {
    moveBoth(600, 400, 1);
    moveBoth(400, 600, 1);
  }
  // Move back to default position
  reset();
}

void Actuator::waitMaxDelay (int leftNewPos, int rightNewPos) {
  int leftDelay = leftServo->calcDelay(leftNewPos);
  int rightDelay = rightServo->calcDelay(rightNewPos);
  delay(max(leftDelay, rightDelay));
}
