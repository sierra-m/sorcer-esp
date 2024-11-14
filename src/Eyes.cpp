#include "Eyes.h"

Eyes::Eyes (Eye *leftEye, Eye *rightEye) {
  this->leftEye = leftEye;
  this->rightEye = rightEye;
  this->start = ((leftEye->start < rightEye->start) ? leftEye->start : rightEye->start);
  this->ledSpan = leftEye->leds + start;
}

void Eyes::reset () {
  this->leftEye->reset();
  this->rightEye->reset();
}

void Eyes::clearAnimation () {
  leftEye->clearAnimation();
  rightEye->clearAnimation();
}

void Eyes::blink (uint16_t stepDelayMillis) {
  leftEye->blink(stepDelayMillis);
  rightEye->blink(stepDelayMillis);
}

void Eyes::open (uint8_t _clearAnimation) {
  leftEye->open(_clearAnimation);
  rightEye->open(_clearAnimation);
}

void Eyes::close (uint8_t _clearAnimation) {
  leftEye->close(_clearAnimation);
  rightEye->close(_clearAnimation);
}

void Eyes::dilate (uint8_t _clearAnimation) {
  leftEye->dilate(_clearAnimation);
  rightEye->dilate(_clearAnimation);
}

void Eyes::contract (uint8_t _clearAnimation) {
  leftEye->contract(_clearAnimation);
  rightEye->contract(_clearAnimation);
}

void Eyes::squint (uint8_t _clearAnimation) {
  leftEye->squint(_clearAnimation);
  rightEye->squint(_clearAnimation);
}

void Eyes::setInfill (uint8_t hasInfill) {
  leftEye->setInfill(hasInfill);
  rightEye->setInfill(hasInfill);
}

void Eyes::dead (uint8_t _clearAnimation) {
  leftEye->dead(_clearAnimation);
  rightEye->dead(_clearAnimation);
}

void Eyes::confused () {
  leftEye->contract(1);
  rightEye->dilate(1);
}

void Eyes::lookLeft (uint8_t _clearAnimation) {
  leftEye->lookLeft(_clearAnimation);
  rightEye->lookLeft(_clearAnimation);
}

void Eyes::lookRight (uint8_t _clearAnimation) {
  leftEye->lookRight(_clearAnimation);
  rightEye->lookRight(_clearAnimation);
}

void Eyes::lookUp (uint8_t _clearAnimation) {
  leftEye->lookUp(_clearAnimation);
  rightEye->lookUp(_clearAnimation);
}

void Eyes::lookDown (uint8_t _clearAnimation) {
  leftEye->lookDown(_clearAnimation);
  rightEye->lookDown(_clearAnimation);
}

void Eyes::angry () {
  leftEye->angry();
  rightEye->angry();
}

void Eyes::happy () {
  leftEye->happy();
  rightEye->happy();
}

void Eyes::caution () {
  leftEye->caution();
  rightEye->caution();
}

void Eyes::setColor (CRGB newColor) {
  leftEye->setColor(newColor);
  rightEye->setColor(newColor);
}

void Eyes::rainbow (uint16_t stepDelayMillis) {
  leftEye->rainbow(stepDelayMillis);
  rightEye->rainbow(stepDelayMillis);
}

void Eyes::spiralDot (uint16_t stepDelayMillis, uint8_t up) {
  leftEye->spiral(stepDelayMillis, up, 1);
  rightEye->spiral(stepDelayMillis, up, 1);
}

void Eyes::spiralLine (uint16_t stepDelayMillis, uint8_t up) {
  leftEye->spiral(stepDelayMillis, up, 0);
  rightEye->spiral(stepDelayMillis, up, 0);
}

void Eyes::update () {
  leftEye->update();
  rightEye->update();
}


