#ifndef EYE_H
#define EYE_H

#include <stdint.h>
#include <FastLED.h>
#include "Arduino.h"

// There are 3 "rings": outer, inner, center dot.
// Data moves from inner dot to outer ring
#define EYE_OUTER_RING_COUNT 12
#define EYE_INNER_RING_COUNT 8
#define EYE_OUTER_RING_START (1 + EYE_INNER_RING_COUNT)
#define EYE_INNER_RING_START 1
#define EYE_DOT_START 0
#define EYE_LED_COUNT (1 + EYE_INNER_RING_COUNT + EYE_OUTER_RING_COUNT)

// Default step delays for animations
#define EYE_BLINK_STEP_DELAY_MS 75
#define EYE_RAINBOW_STEP_DELAY_MS 100
#define EYE_SPIRAL_STEP_DELAY_MS 50

// Static drawing array sizes
#define EYE_CLOSED_IDXS_SIZE 5
#define EYE_CLOSED_REG_PUPIL_SIZE 3
#define EYE_CLOSED_REG_PUPIL_START_IDX 1

#define EYE_SQUINT_EXT_IDXS_SIZE 3
#define EYE_BLINK_STEP0_IDXS_SIZE 2
#define EYE_BLINK_STEP1_IDXS_SIZE 4
#define EYE_DEAD_IDXS_SIZE 13
#define EYE_LOOK_REG_IDXS_SIZE 6
#define EYE_LOOK_LRG_IDXS_SIZE 2


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

typedef enum {
  ANIMATION_NONE,
  ANIMATION_BLINKING,
  ANIMATION_RAINBOW,
  ANIMATION_SPIRAL_DOT,
  ANIMATION_SPIRAL_LINE
} AnimationType;

typedef struct {
  uint8_t step;
} AnimationBlinkState;

typedef struct {
  uint8_t outerHue;
  uint8_t innerHue;
  uint8_t dotHue;
  uint8_t outerClockwise;
} AnimationRainbowState;

typedef struct {
  uint8_t position;
  uint8_t up;
} AnimationSpiralState;

typedef union {
  AnimationBlinkState blink;
  AnimationRainbowState rainbow;
  AnimationSpiralState spiral;
} AnimationStateU;

typedef struct {
  AnimationType type;
  AnimationStateU u;
  uint16_t frameDelayMillis;
} AnimationState;

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

    // Look directions for regular pupil size
    static const uint8_t eyeLookLeftIdxs[EYE_LOOK_REG_IDXS_SIZE];

    static const uint8_t eyeLookRightIdxs[EYE_LOOK_REG_IDXS_SIZE];

    static const uint8_t eyeLookUpIdxs[EYE_LOOK_REG_IDXS_SIZE];

    static const uint8_t eyeLookDownIdxs[EYE_LOOK_REG_IDXS_SIZE];

    // Extras for look directions for large pupil size
    static const uint8_t eyeLookLeftLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE];
    
    static const uint8_t eyeLookRightLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE];
    
    static const uint8_t eyeLookUpLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE];
    
    static const uint8_t eyeLookDownLrgIdxs[EYE_LOOK_LRG_IDXS_SIZE];

    Eye (CRGB *leds, int start, CRGB defaultColor);
    // Reset to default size and color
    void reset ();
    // Cancel any current animation
    void clearAnimation ();

    // Static drawings
    // ============================
    // Clear eye
    void clear (uint8_t _clearAnimation = 0);
    // Fill entire eye
    void fill (uint8_t _clearAnimation = 0);
    // Write ring
    void writeRing (RingArea ring, CRGB newColor);
    // Open the eye (default pupil size)
    void open (uint8_t _clearAnimation = 0);
    // Close the eye (horizontal line)
    void close (uint8_t _clearAnimation = 0);
    // Squint the eye (horizontal line and half oval)
    void squint (uint8_t _clearAnimation = 0);
    // Dilate pupil (outer circle)
    void dilate (uint8_t _clearAnimation = 0);
    // Contract pupil (inner circle and dot)
    void contract (uint8_t _clearAnimation = 0);
    // Set pupil infill
    void setInfill (uint8_t hasInfill);
    // Display dead symbol (red x)
    void dead (uint8_t _clearAnimation = 0);
    // Look left
    void lookLeft (uint8_t _clearAnimation = 0);
    // Look right
    void lookRight (uint8_t _clearAnimation = 0);
    // Look up
    void lookUp (uint8_t _clearAnimation = 0);
    // Look down
    void lookDown (uint8_t _clearAnimation = 0);
    // Step 0 for blink animation
    void blinkStep0 (BlinkState blinkState);
    // Step 1 for blink animation
    void blinkStep1 (BlinkState blinkState);

    // Animations
    // ============================
    // Blink the eye (default pattern to horizontal line and back).
    // Currently only supported for regular size pupil. Blocking call
    void blink (uint16_t stepDelayMillis = EYE_BLINK_STEP_DELAY_MS);
    // Display a rainbow gradient animation
    void rainbow (uint16_t stepDelayMillis = EYE_RAINBOW_STEP_DELAY_MS);
    // Spiral animation
    void spiral (uint16_t stepDelayMillis = EYE_SPIRAL_STEP_DELAY_MS, uint8_t up = 1, uint8_t clearBehind = 1);
    // Handle blink animation updates
    void handleBlinkUpdate ();
    // Handle rainbow animation updates
    void handleRainbowUpdate ();
    // Handle spiral animation updates
    void handleSpiralUpdate (uint8_t clearBehind);
    // Run animation updates
    void update ();

    // Color setting
    // ============================
    // Set color to red
    void angry ();
    // Set color to green
    void happy ();
    // Set color to yellow
    void caution ();
    // Set color to custom value
    void setColor (CRGB newColor);
  private:
    // State of any current animation
    AnimationState animationState;
    // Last time updates occurred
    unsigned long lastTimeMillis;
};

#endif