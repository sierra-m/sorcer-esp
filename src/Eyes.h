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
    // Blink both eyes
    void blink ();
    // Open both eyes
    void open ();
    // Close both eyes
    void close ();
    // Dilate both eyes
    void dilate ();
    // Contract both eyes
    void contract ();
    // Squint both eyes
    void squint ();
    // Set infill in both eyes
    void setInfill (uint8_t hasInfill);
    // Then die
    void dead ();
    // Set both eyes to rainbow
    void rainbow ();
    // Dilate one eye to display confusion
    void confused ();
    // Set color to red
    void angry ();
    // Set color to green
    void happy ();
    // Set color to yellow
    void caution ();
    // Set both eyes to one color
    void setColor (CRGB newColor);
    // Send dot spiralling across each eye
    void spiralDot ();
    // Spiral line across each eye
    void spiralLine ();
};

#endif