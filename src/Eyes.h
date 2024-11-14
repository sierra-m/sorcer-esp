#ifndef EYES_H
#define EYES_H

#define EYES_LED_COUNT (EYE_LED_COUNT * 2)

#define EYES_SPIRAL_STEP_DELAY_MS 50

#include <stdint.h>
#include "Eye.h"
#include "Arduino.h"

// This class represents control of both eye LED arrays. Most
// methods provided are simply for convenience, but there are
// some unique drawings/animations
class Eyes {
  public:
    Eye *leftEye;
    Eye *rightEye;
    int start;
    CRGB *ledSpan;

    Eyes (Eye *leftEye, Eye *rightEye);
    // Reset both eyes
    void reset ();
    // Clear any animation on both eyes
    void clearAnimation ();
    // Blink both eyes
    void blink (uint16_t stepDelayMillis = EYE_BLINK_STEP_DELAY_MS);
    // Open both eyes
    void open (uint8_t _clearAnimation = 0);
    // Close both eyes
    void close (uint8_t _clearAnimation = 0);
    // Dilate both eyes
    void dilate (uint8_t _clearAnimation = 0);
    // Contract both eyes
    void contract (uint8_t _clearAnimation = 0);
    // Squint both eyes
    void squint (uint8_t _clearAnimation = 0);
    // Set infill in both eyes
    void setInfill (uint8_t hasInfill);
    // Then die.
    void dead (uint8_t _clearAnimation = 0);
    // Dilate one eye to display confusion
    void confused ();
    // Both look left
    void lookLeft (uint8_t _clearAnimation = 0);
    // Both look right
    void lookRight (uint8_t _clearAnimation = 0);
    // Both look up
    void lookUp (uint8_t _clearAnimation = 0);
    // Both look down
    void lookDown (uint8_t _clearAnimation = 0);
    // Set color to red
    void angry ();
    // Set color to green
    void happy ();
    // Set color to yellow
    void caution ();
    // Set both eyes to one color
    void setColor (CRGB newColor);
    // Set both eyes to rainbow animation
    void rainbow (uint16_t stepDelayMillis = EYE_RAINBOW_STEP_DELAY_MS);
    // Send dot spiralling across each eye
    void spiralDot (uint16_t stepDelayMillis = EYE_SPIRAL_STEP_DELAY_MS, uint8_t up = 1);
    // Spiral line across each eye
    void spiralLine (uint16_t stepDelayMillis = EYE_SPIRAL_STEP_DELAY_MS, uint8_t up = 1);
    // Update both eye animations
    void update ();
};

#endif