#include "Actuator.h"

Actuator::Actuator (ServoDS3218 *leftServo, ServoDS3218 *rightServo, ExtendDirection leftServoDir = CLOCKWISE) {
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

void Actuator::retract (uint8_t blocking) {
  leftServo->moveStart();
  rightServo->moveStart();
  if (blocking) {
    waitMaxDelay(0, 0);
  }
}

void Actuator::extend (uint8_t blocking) {
  leftServo->moveEnd();
  rightServo->moveEnd();
  if (blocking) {
    waitMaxDelay(1000, 1000);
  }
}

void Actuator::waitMaxDelay (int leftNewPos, int rightNewPos) {
  int leftDelay = leftServo->calcDelay(leftNewPos);
  int rightDelay = rightServo->calcDelay(rightNewPos);
  delay(max(leftDelay, rightDelay));
}
