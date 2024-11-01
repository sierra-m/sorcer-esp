#include "Eyes.h"

Eyes::Eyes (Eye *leftEye, Eye *rightEye) {
  this->leftEye = leftEye;
  this->rightEye = rightEye;
}

void Eyes::reset () {
  this->leftEye->reset();
  this->rightEye->reset();
}

void Eyes::blink () {
  leftEye->blinkStep0(BLINK_CLOSING);
  rightEye->blinkStep0(BLINK_CLOSING);
  FastLED.show();
  delay(EYE_BLINK_STEP_DELAY_MS);

  leftEye->blinkStep1(BLINK_CLOSING);
  rightEye->blinkStep1(BLINK_CLOSING);
  FastLED.show();
  delay(EYE_BLINK_STEP_DELAY_MS);

  leftEye->blinkStep1(BLINK_OPENING);
  rightEye->blinkStep1(BLINK_OPENING);
  FastLED.show();
  delay(EYE_BLINK_STEP_DELAY_MS);

  leftEye->blinkStep0(BLINK_OPENING);
  rightEye->blinkStep0(BLINK_OPENING);
}

void Eyes::open () {
  leftEye->open();
  rightEye->open();
}

void Eyes::close () {
  leftEye->close();
  rightEye->close();
}

void Eyes::squint () {
  leftEye->squint();
  rightEye->squint();
}

void Eyes::confused () {
  leftEye->contract();
  rightEye->dilate();
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