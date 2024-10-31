#ifndef EYE_H
#define EYE_H

#include <stdint.h>
#include <FastLED.h>

#define BLINK_DELAY 200

typedef enum {
  PUPIL_REG,
  PUPIL_LARGE
} PupilSize;

class Eye {
  public:
    CRGB *leds;
    int start;
    int end;
    int size;

    CRGB currentColor;
    PupilSize pupilSize;

    Eye (CRGB *leds, int start, int size, CRGB defaultColor);
    // Reset to default size and color
    void reset ();

    // Static drawings
    // ============================
    // Open the eye (default pupil size)
    void open ();
    // Close the eye (horizontal line)
    void close ();
    // Squint the eye (horizontal line and half oval)
    void squint ();
    // Dilate pupil (outer circle)
    void dilate ();
    // Contract pupil (inner circle and dot)
    void contract ();

    // Animations
    // ============================
    // Blink the eye (default pattern to horizontal line and back)
    void blink ();
    // Roll eye
    void roll (uint8_t clockwise = 1);

    // Color setting
    // ============================
    // Set color to red
    void angry ();
    // Set color to green
    void happy ();
    // Set color to yellow
    void caution ();
    void setColor (CRGB newColor);
};

#endif