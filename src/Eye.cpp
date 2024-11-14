#include <stdint.h>
#include "esp32-hal.h"
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

const uint8_t Eye::eyeLookLeftIdxs[EYE_LOOK_REG_IDXS_SIZE] = {
  2 + EYE_OUTER_RING_START,
  3 + EYE_OUTER_RING_START,
  4 + EYE_OUTER_RING_START,
  1 + EYE_INNER_RING_START,
  2 + EYE_INNER_RING_START,
  3 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeLookRightIdxs[EYE_LOOK_REG_IDXS_SIZE] = {
  8 + EYE_OUTER_RING_START,
  9 + EYE_OUTER_RING_START,
  10 + EYE_OUTER_RING_START,
  5 + EYE_INNER_RING_START,
  6 + EYE_INNER_RING_START,
  7 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeLookUpIdxs[EYE_LOOK_REG_IDXS_SIZE] = {
  5 + EYE_OUTER_RING_START,
  6 + EYE_OUTER_RING_START,
  7 + EYE_OUTER_RING_START,
  3 + EYE_INNER_RING_START,
  4 + EYE_INNER_RING_START,
  5 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeLookDownIdxs[EYE_LOOK_REG_IDXS_SIZE] = {
  0 + EYE_OUTER_RING_START,
  1 + EYE_OUTER_RING_START,
  11 + EYE_OUTER_RING_START,
  0 + EYE_INNER_RING_START,
  1 + EYE_INNER_RING_START,
  7 + EYE_INNER_RING_START
};

const uint8_t Eye::eyeLookLeftLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE] = {
  1 + EYE_OUTER_RING_START,
  5 + EYE_OUTER_RING_START
};

const uint8_t Eye::eyeLookRightLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE] = {
  7 + EYE_OUTER_RING_START,
  11 + EYE_OUTER_RING_START
};

const uint8_t Eye::eyeLookUpLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE] = {
  4 + EYE_OUTER_RING_START,
  8 + EYE_OUTER_RING_START
};

const uint8_t Eye::eyeLookDownLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE] = {
  2 + EYE_OUTER_RING_START,
  10 + EYE_OUTER_RING_START
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

void Eye::clearAnimation () {
  animationState.type = ANIMATION_NONE;
}

void Eye::clear (uint8_t _clearAnimation) {
  if (_clearAnimation) {
    clearAnimation();
  }
  // Add start to `leds` pointer to target specific range
  fill_solid(leds + start, EYE_LED_COUNT, 0);
}

void Eye::fill (uint8_t _clearAnimation) {
  if (_clearAnimation) {
    clearAnimation();
  }
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
void Eye::open (uint8_t _clearAnimation) {
  if (_clearAnimation) {
    clearAnimation();
  }
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

void Eye::close (uint8_t _clearAnimation) {
  if (_clearAnimation) {
    clearAnimation();
  }
  writeRing(RING_OUTER, 0);
  writeRing(RING_INNER, 0);
  int startPos = ((pupilSize == PUPIL_REG) ? EYE_CLOSED_REG_PUPIL_START_IDX : 0);
  int drawSize = ((pupilSize == PUPIL_REG) ? EYE_CLOSED_REG_PUPIL_SIZE : EYE_CLOSED_IDXS_SIZE);
  for (int i = startPos; i < drawSize + startPos; i++) {
    leds[Eye::eyeClosedIdxs[i] + start] = currentColor;
  }
}

void Eye::squint (uint8_t _clearAnimation) {
  // First close eye, then draw extra bits
  close(_clearAnimation);
  for (int i = 0; i < 3; i++) {
    leds[Eye::eyeSquintExtIdxs[i] + start] = currentColor;
  }
}

void Eye::dilate (uint8_t _clearAnimation) {
  pupilSize = PUPIL_LARGE;
  open(_clearAnimation);
}

void Eye::contract (uint8_t _clearAnimation) {
  pupilSize = PUPIL_REG;
  open(_clearAnimation);
}

void Eye::setInfill (uint8_t hasInfill) {
  clearAnimation();
  pupilInfill = (hasInfill ? PUPIL_INFILL : PUPIL_NO_INFILL);
  open();
}

void Eye::dead (uint8_t _clearAnimation) {
  clear(_clearAnimation);
  for (int i = 0; i < EYE_DEAD_IDXS_SIZE; i++) {
    leds[Eye::eyeDeadIdxs[i] + start] = CRGB(0xff0000);
  }
}

void Eye::lookLeft (uint8_t _clearAnimation) {
  clear(_clearAnimation);
  for (int i = 0; i < EYE_LOOK_REG_IDXS_SIZE; i++) {
    leds[Eye::eyeLookLeftIdxs[i] + start] = currentColor;
  }
  if (pupilSize == PUPIL_LARGE) {
    for (int i = 0; i < EYE_LOOK_LRG_IDXS_SIZE; i++) {
      leds[Eye::eyeLookLeftLrgIdxs[i] + start] = currentColor;
    }
  }
}

void Eye::lookRight (uint8_t _clearAnimation) {
  clear(_clearAnimation);
  for (int i = 0; i < EYE_LOOK_REG_IDXS_SIZE; i++) {
    leds[Eye::eyeLookRightIdxs[i] + start] = currentColor;
  }
  if (pupilSize == PUPIL_LARGE) {
    for (int i = 0; i < EYE_LOOK_LRG_IDXS_SIZE; i++) {
      leds[Eye::eyeLookRightLrgIdxs[i] + start] = currentColor;
    }
  }
}

void Eye::lookUp (uint8_t _clearAnimation) {
  clear(_clearAnimation);
  for (int i = 0; i < EYE_LOOK_REG_IDXS_SIZE; i++) {
    leds[Eye::eyeLookUpIdxs[i] + start] = currentColor;
  }
  if (pupilSize == PUPIL_LARGE) {
    for (int i = 0; i < EYE_LOOK_LRG_IDXS_SIZE; i++) {
      leds[Eye::eyeLookUpLrgIdxs[i] + start] = currentColor;
    }
  }
}

void Eye::lookDown (uint8_t _clearAnimation) {
  clear(_clearAnimation);
  for (int i = 0; i < EYE_LOOK_REG_IDXS_SIZE; i++) {
    leds[Eye::eyeLookDownIdxs[i] + start] = currentColor;
  }
  if (pupilSize == PUPIL_LARGE) {
    for (int i = 0; i < EYE_LOOK_LRG_IDXS_SIZE; i++) {
      leds[Eye::eyeLookDownLrgIdxs[i] + start] = currentColor;
    }
  }
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
void Eye::blink (uint16_t stepDelayMillis) {
  open();
  animationState.type = ANIMATION_BLINKING;
  animationState.u.blink.step = 0;
  animationState.frameDelayMillis = stepDelayMillis;
}

void Eye::rainbow (uint16_t stepDelayMillis) {
  animationState.type = ANIMATION_RAINBOW;
  animationState.u.rainbow.outerHue = (uint8_t)random(256);
  animationState.u.rainbow.innerHue = (uint8_t)random(256);
  animationState.u.rainbow.dotHue = (uint8_t)random(256);
  animationState.u.rainbow.outerClockwise = (uint8_t)random(2);
  animationState.frameDelayMillis = stepDelayMillis;
}

void Eye::spiral (uint16_t stepDelayMillis, uint8_t up, uint8_t clearBehind) {
  clear();
  animationState.type = (clearBehind ? ANIMATION_SPIRAL_DOT : ANIMATION_SPIRAL_LINE);
  animationState.u.spiral.position = 0;
  animationState.u.spiral.up = up;
  animationState.frameDelayMillis = stepDelayMillis;
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

void Eye::handleBlinkUpdate () {
  if (pupilSize == PUPIL_REG) {
    switch (animationState.u.blink.step) {
      case 0:
        blinkStep0(BLINK_CLOSING);
        break;
      case 1:
        blinkStep1(BLINK_CLOSING);
        break;
      case 2:
        // do nothing
        break;
      case 3:
        blinkStep1(BLINK_OPENING);
        break;
      case 4:
        blinkStep0(BLINK_OPENING);
        // Exit condition
        clearAnimation();
        break;
      default:
        clearAnimation();
        break;
    }
  } else {
    switch (animationState.u.blink.step) {
      case 0:
        close();
        break;
      case 1:  // fallthrough
      case 2:  // fallthrough
      case 3:
        // do nothing
        break;
      case 4:
        open();
        // Exit condition
        clearAnimation();
        break;
      default:
        clearAnimation();
        break;
    }
  }
  animationState.u.blink.step += 1;
}

void Eye::handleRainbowUpdate () {
  CHSV hsv;
  hsv.hue = animationState.u.rainbow.dotHue;
  hsv.val = 255;
  hsv.sat = 240;
  // Set dot to single hue
  leds[start + EYE_DOT_START] = hsv;
  // Fill inner dot and inner ring as one
  fill_rainbow(leds + start, EYE_INNER_RING_COUNT, animationState.u.rainbow.innerHue, 255 / EYE_INNER_RING_COUNT);
  // Fille outer ring separately
  fill_rainbow(leds + start + EYE_OUTER_RING_START, EYE_OUTER_RING_COUNT, animationState.u.rainbow.outerHue, 255 / EYE_OUTER_RING_COUNT);

  // Make rings move opposite each other
  if (animationState.u.rainbow.outerClockwise) {
    animationState.u.rainbow.outerHue -= 1;
    animationState.u.rainbow.innerHue += 1;
    animationState.u.rainbow.dotHue -= 1;
  } else {
    animationState.u.rainbow.outerHue += 1;
    animationState.u.rainbow.innerHue -= 1;
    animationState.u.rainbow.dotHue += 1;
  }
  // No exit condition - must be overridden
}

void Eye::handleSpiralUpdate (uint8_t clearBehind) {
  // Calc direction delta
  int8_t delta = (animationState.u.spiral.up ? 1 : -1);

  // For all but last, run spiral
  if (animationState.u.spiral.position < EYE_LED_COUNT) {
    leds[start + animationState.u.spiral.position] = currentColor;
    // For dot spiral, clear the previous led
    if (clearBehind && (animationState.u.spiral.position > 0)) {
      leds[start + (uint8_t)(animationState.u.spiral.position + delta)] = 0;
    }
  } else {
    // Clear last led
    leds[start + EYE_LED_COUNT - 1] = 0;
    // Exit condition
    clearAnimation();
  }
  animationState.u.spiral.position += delta;
}

void Eye::update () {
  if (animationState.type != ANIMATION_NONE) {
    if ((millis() - lastTimeMillis) > animationState.frameDelayMillis) {
      switch (animationState.type) {
        case ANIMATION_BLINKING:
          handleBlinkUpdate();
          break;
        case ANIMATION_RAINBOW:
          handleRainbowUpdate();
          break;
        case ANIMATION_SPIRAL_DOT:
          handleSpiralUpdate(1);
          break;
        case ANIMATION_SPIRAL_LINE:
          handleSpiralUpdate(0);
          break;
      }
      FastLED.show();
      lastTimeMillis = millis();
    }
  }
}