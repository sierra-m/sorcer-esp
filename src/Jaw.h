#ifndef JAW_H
#define JAW_H

#include <stdint.h>
#include "Arduino.h"
#include <FastLED.h>
#include "MicroServoSG90.h"

class Jaw {
  public:
    MicroServoSG90 *jawServo;
    CRGB *leds;
    int ledCount;

    CRGB defaultColor;
    CRGB currentColor;

    Jaw (MicroServoSG90 *jawServo, CRGB *leds, int ledCount, CRGB defaultColor);
    // Open mouth
    void open (uint8_t blocking = 0);
    // Close mouth
    void close (uint8_t blocking = 0);
    // Move jaw up and down rapidly
    void laugh (int count = 3);
    // Set color of mouth
    void setColor (CRGB newColor);
    // Reset servo and strip
    void reset ();
    // Reset strip color
    void resetColor ();
};

#endif