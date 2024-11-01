#ifndef EYES_H
#define EYES_H

#include <stdint.h>
#include "Eye.h"
#include "Arduino.h"


class Eyes {
  public:
    Eye *leftEye;
    Eye *rightEye;

    Eyes (Eye *leftEye, Eye *rightEye);
    // Reset both eyes
    void reset ();
    // Blink both eyes
    void blink ();
    // Close both eyes
    void close ();
    // Squint both eyes
    void squint ();
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
};

#endif