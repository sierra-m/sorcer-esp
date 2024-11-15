#include "Jaw.h"

Jaw::Jaw (MicroServoSG90 *jawServo, CRGB *leds, int ledCount, CRGB defaultColor) {
  this->jawServo = jawServo;
  this->leds = leds;
  this->ledCount = ledCount;
  this->defaultColor = defaultColor;
  this->currentColor = defaultColor;
}

void Jaw::open (uint8_t blocking) {
  jawServo->setPos(400, blocking);
  fill_solid(leds, ledCount, currentColor);
}

void Jaw::close (uint8_t blocking) {
  jawServo->setPos(0, blocking);
  fill_solid(leds, ledCount, 0);
}

void Jaw::laugh (int count) {
  for (int i = 0; i < count; i++) {
    open(1);
    close(1);
  }
}

void Jaw::setColor (CRGB newColor) {
  currentColor = newColor;
  fill_solid(leds, ledCount, newColor);
}

void Jaw::reset () {
  close(1);
}

void Jaw::resetColor () {
  currentColor = defaultColor;
  fill_solid(leds, ledCount, currentColor);
}