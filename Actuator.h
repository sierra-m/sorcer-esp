#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <ctype.h>
#include "Arduino.h"

#define MAX_BIT_NUM 14

#define PULSE_WIDTH_MAX 2150
#define PULSE_WIDTH_MID 1250
#define PULSE_WIDTH_MIN 350

typedef enum {
  CLOCKWISE,
  COUNTER_CLOCKWISE
} ExtendDirection;

class Actuator {
  public:
    uint8_t pin;
    uint8_t channel;

    int startPos;
    int endPos;

    int currentPos;

    ExtendDirection direction;
    //void (*writeChannel)(uint8_t, uint32_t);

    Actuator (uint8_t pin, uint8_t channel, ExtendDirection direction = CLOCKWISE);
    // Set the pulse width directly
    void setPulseWidth (int width);
    // Set a position between 0 and 1000
    void setPos (int pos);
    void retract (int pos = -1);
    void extend (int pos = -1);
};

#endif