#include "Eye.h"

const uint8_t Eye::eyeClosedIdxs[EYE_CLOSED_IDXS_SIZE] = {
  3 + EYE_OUTER_RING_START,
  2 + EYE_INNER_RING_START,
  0 + EYE_DOT_START,
  6 + EYE_INNER_RING_START,
  9 + EYE_OUTER_RING_START
};

const uint8_t Eye::eyeSquintExtIdxs[EYE_SQUINT_EXT_IDXS_SIZE] = {
  1 + EYE_INNER_RING_START,
  0 + EYE_INNER_RING_START,
  7 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeBlinkStep0Idxs[EYE_BLINK_STEP0_IDXS_SIZE] = {
  0 + EYE_INNER_RING_START,
  4 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeBlinkStep1Idxs[EYE_BLINK_STEP1_IDXS_SIZE] = {
  1 + EYE_INNER_RING_START,
  3 + EYE_INNER_RING_START,
  5 + EYE_INNER_RING_START,
  7 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeDeadIdxs[EYE_DEAD_IDXS_SIZE] = {
  1 + EYE_OUTER_RING_START,
  2 + EYE_OUTER_RING_START,
  4 + EYE_OUTER_RING_START,
  5 + EYE_OUTER_RING_START,
  7 + EYE_OUTER_RING_START,
  8 + EYE_OUTER_RING_START,
  10 + EYE_OUTER_RING_START,
  11 + EYE_OUTER_RING_START,
  1 + EYE_INNER_RING_START,
  3 + EYE_INNER_RING_START,
  5 + EYE_INNER_RING_START,
  7 + EYE_INNER_RING_START,
  0 + EYE_DOT_START
};

Eye::Eye (CRGB *leds, int start, CRGB defaultColor) {
  this->leds = leds;
  this->start = start;
  this->end = start + EYE_LED_COUNT - 1;
  this->defaultColor = defaultColor;
  this->currentColor = defaultColor;
  this->pupilInfill = PUPIL_INFILL;
}

void Eye::reset () {
  currentColor = defaultColor;
  pupilSize = PUPIL_REG;
  pupilInfill = PUPIL_INFILL;
  open();
}

void Eye::clear () {
  // Add start to `leds` pointer to target specific range
  fill_solid(leds + start, EYE_LED_COUNT, 0);
}

void Eye::fill () {
  fill_solid(leds + start, EYE_LED_COUNT, currentColor);
}

void Eye::writeRing (RingArea ring, CRGB newColor) {
  switch (ring) {
    case RING_DOT:
      leds[start + EYE_DOT_START] = newColor;
      break;
    case RING_INNER:
      fill_solid(leds + (start + EYE_INNER_RING_START), EYE_INNER_RING_COUNT, newColor);
      break;
    case RING_OUTER:
      fill_solid(leds + (start + EYE_OUTER_RING_START), EYE_OUTER_RING_COUNT, newColor);
      break;
  }
}

// Static drawings
// ============================
void Eye::open () {
  CRGB dotColor, innerColor, outerColor;
  dotColor = ((pupilInfill == PUPIL_INFILL) ? currentColor : 0);
  if (pupilSize == PUPIL_REG) {
    innerColor = currentColor;
    outerColor = 0;
  } else {
    innerColor = dotColor;
    outerColor = currentColor;
  }
  writeRing(RING_DOT, dotColor);
  writeRing(RING_INNER, innerColor);
  writeRing(RING_OUTER, outerColor);
}

void Eye::close () {
  writeRing(RING_OUTER, 0);
  writeRing(RING_INNER, 0);
  int startPos = ((pupilSize == PUPIL_REG) ? EYE_CLOSED_REG_PUPIL_START_IDX : 0);
  int drawSize = ((pupilSize == PUPIL_REG) ? EYE_CLOSED_REG_PUPIL_SIZE : EYE_CLOSED_IDXS_SIZE);
  for (int i = startPos; i < drawSize + startPos; i++) {
    leds[Eye::eyeClosedIdxs[i] + start] = currentColor;
  }
}

void Eye::squint () {
  // First close eye, then draw extra bits
  close();
  for (int i = 0; i < 3; i++) {
    leds[Eye::eyeSquintExtIdxs[i] + start] = currentColor;
  }
}

void Eye::dilate () {
  pupilSize = PUPIL_LARGE;
  open();
}

void Eye::contract () {
  pupilSize = PUPIL_REG;
  open();
}

void Eye::setInfill (uint8_t hasInfill) {
  pupilInfill = (hasInfill ? PUPIL_INFILL : PUPIL_NO_INFILL);
  open();
}

void Eye::dead () {
  clear();
  for (int i = 0; i < EYE_DEAD_IDXS_SIZE; i++) {
    leds[Eye::eyeDeadIdxs[i] + start] = CRGB(0xff0000);
  }
}

void Eye::rainbow () {
  fill_rainbow(leds + start, EYE_LED_COUNT, 0, 255 / EYE_LED_COUNT);
}

void Eye::blinkStep0 (BlinkState blinkState) {
  if (pupilSize == PUPIL_REG) {
    CRGB newColor = ((blinkState == BLINK_CLOSING) ? 0 : currentColor);
    for (int i = 0; i < EYE_BLINK_STEP0_IDXS_SIZE; i++) {
      leds[Eye::eyeBlinkStep0Idxs[i] + start] = newColor;
    }
  } else {
    if (blinkState == BLINK_CLOSING) {
      close();
    } else {
      open();
    }
  }
}

void Eye::blinkStep1 (BlinkState blinkState) {
  if (pupilSize == PUPIL_REG) {
    CRGB newColor = ((blinkState == BLINK_CLOSING) ? 0 : currentColor);
    for (int i = 0; i < EYE_BLINK_STEP1_IDXS_SIZE; i++) {
      leds[Eye::eyeBlinkStep1Idxs[i] + start] = newColor;
    }
    if (blinkState == BLINK_CLOSING) {
      // dot should always display when closed
      writeRing(RING_DOT, currentColor);
    } else {
      // Clear dot for no infill when opening
      if (pupilInfill == PUPIL_NO_INFILL) {
        writeRing(RING_DOT, 0);
      }
    }
  } else {
    close();
  }
}

// Animations
// ============================
void Eye::blink () {
  // assumes eye starts open
  if (pupilSize == PUPIL_REG) {
    blinkStep0(BLINK_CLOSING);
    FastLED.show();
    delay(EYE_BLINK_STEP_DELAY_MS);

    blinkStep1(BLINK_CLOSING);
    FastLED.show();
    delay(EYE_BLINK_STEP_DELAY_MS);
    delay(EYE_BLINK_STEP_DELAY_MS);

    blinkStep1(BLINK_OPENING);
    FastLED.show();
    delay(EYE_BLINK_STEP_DELAY_MS);

    blinkStep0(BLINK_OPENING);
    FastLED.show();
  } else {
    close();
    FastLED.show();
    delay(EYE_BLINK_STEP_DELAY_MS * 3);
    open();
    FastLED.show();
  }
}

// Color setting
void Eye::angry () {
  currentColor = CRGB::Red;
  open();
}

void Eye::happy () {
  currentColor = CRGB::Green;
  open();
}

void Eye::caution () {
  currentColor = CRGB::Yellow;
  open();
}

void Eye::setColor (CRGB newColor) {
  currentColor = newColor;
  open();
}