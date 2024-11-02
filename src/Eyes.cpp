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

void Eyes::blink () {
  leftEye->blinkStep0(BLINK_CLOSING);
  rightEye->blinkStep0(BLINK_CLOSING);
  FastLED.show();
  delay(EYE_BLINK_STEP_DELAY_MS);

  leftEye->blinkStep1(BLINK_CLOSING);
  rightEye->blinkStep1(BLINK_CLOSING);
  FastLED.show();
  delay(EYE_BLINK_STEP_DELAY_MS);
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

void Eyes::dilate () {
  leftEye->dilate();
  rightEye->dilate();
}

void Eyes::contract () {
  leftEye->contract();
  rightEye->contract();
}

void Eyes::squint () {
  leftEye->squint();
  rightEye->squint();
}

void Eyes::setInfill (uint8_t hasInfill) {
  leftEye->setInfill(hasInfill);
  rightEye->setInfill(hasInfill);
}

void Eyes::dead () {
  leftEye->dead();
  rightEye->dead();
}

void Eyes::rainbow () {
  leftEye->rainbow();
  rightEye->rainbow();
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

void Eyes::spiralDot () {
  fill_solid(ledSpan, EYES_LED_COUNT, 0);
  for (int i = 0; i < EYES_LED_COUNT; i++) {
    ledSpan[i] = leftEye->currentColor;
    if (i > 0) {
      ledSpan[i - 1] = 0;
    }
    FastLED.show();
    delay(EYES_SPIRAL_STEP_DELAY_MS);
  }
  ledSpan[EYES_LED_COUNT - 1] = 0;
}

void Eyes::spiralLine () {
  fill_solid(ledSpan, EYES_LED_COUNT, 0);
  for (int i = 0; i < EYES_LED_COUNT; i++) {
    ledSpan[i] = leftEye->currentColor;
    FastLED.show();
    delay(EYES_SPIRAL_STEP_DELAY_MS);
  }
}


