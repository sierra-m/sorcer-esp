#include "Jaw.h"

Jaw::Jaw (MicroServoSG90 *jawServo) {
  this->jawServo = jawServo;
}

void Jaw::open (uint8_t blocking) {
  jawServo->setPos(400, blocking);
}

void Jaw::close (uint8_t blocking) {
  jawServo->setPos(600, blocking);
}

void Jaw::laugh (int count) {
  for (int i = 0; i < count; i++) {
    open(1);
    close(1);
  }
}