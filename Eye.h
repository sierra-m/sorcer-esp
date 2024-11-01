#ifndef EYE_H
#define EYE_H

#include <stdint.h>
#include <FastLED.h>
#include "Arduino.h"

// There are 3 "rings": outer, inner, center dot.
// Data moves from inner dot to outer ring
#define EYE_OUTER_RING_COUNT 12
#define EYE_INNER_RING_COUNT 8
#define EYE_OUTER_RING_START 1 + EYE_INNER_RING_COUNT
#define EYE_INNER_RING_START 1
#define EYE_DOT_START 0
#define EYE_BLINK_STEP_DELAY_MS 50
#define EYE_LED_COUNT 1 + EYE_INNER_RING_COUNT + EYE_OUTER_RING_COUNT

// Static drawing array sizes
#define EYE_CLOSED_IDXS_SIZE 5
#define EYE_CLOSED_REG_PUPIL_SIZE 3
#define EYE_CLOSED_REG_PUPIL_START_IDX 0

#define EYE_SQUINT_EXT_IDXS_SIZE 3
#define EYE_BLINK_STEP0_IDXS_SIZE 2
#define EYE_BLINK_STEP1_IDXS_SIZE 4
#define EYE_DEAD_IDXS_SIZE 13


typedef enum {
  PUPIL_REG,
  PUPIL_LARGE
} PupilSize;

typedef enum {
  PUPIL_NO_INFILL,
  PUPIL_INFILL
} PupilInfill;

typedef enum {
  RING_DOT,
  RING_INNER,
  RING_OUTER
} RingArea;

typedef enum {
  BLINK_CLOSING,
  BLINK_OPENING
} BlinkState;

class Eye {
  public:
    CRGB *leds;
    int start;
    int end;

    CRGB defaultColor, currentColor;
    PupilSize pupilSize;
    PupilInfill pupilInfill;

    static const uint8_t eyeClosedIdxs[EYE_CLOSED_IDXS_SIZE];

    static const uint8_t eyeSquintExtIdxs[EYE_SQUINT_EXT_IDXS_SIZE];

    static const uint8_t eyeBlinkStep0Idxs[EYE_BLINK_STEP0_IDXS_SIZE];

    static const uint8_t eyeBlinkStep1Idxs[EYE_BLINK_STEP1_IDXS_SIZE];

    static const uint8_t eyeDeadIdxs[EYE_DEAD_IDXS_SIZE];

    Eye (CRGB *leds, int start, CRGB defaultColor);
    // Reset to default size and color
    void reset ();

    // Static drawings
    // ============================
    // Clear eye
    void clear ();
    // Write ring
    void writeRing (RingArea ring, CRGB newColor);
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
    // Set pupil infill
    void setInfill (uint8_t hasInfill);
    // Display dead symbol (red x)
    void dead ();
    // Display a rainbow gradient
    void rainbow ();
    // Step 0 for blink animation
    void blinkStep0 (BlinkState blinkState);
    // Step 1 for blink animation
    void blinkStep1 (BlinkState blinkState);

    // Animations
    // ============================
    // Blink the eye (default pattern to horizontal line and back).
    // Currently only supported for regular size pupil. Blocking call
    void blink ();

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