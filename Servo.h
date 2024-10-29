#ifndef SERVO_H
#define SERVO_H

#include <ctype.h>
#include "Arduino.h"

#define SERVO_MAX_BIT_NUM 14

#define SERVO_PULSE_WIDTH_MAX 2050
#define SERVO_PULSE_WIDTH_MID 1250
#define SERVO_PULSE_WIDTH_MIN 350

class Servo {
  public:
    uint8_t pin;
    uint8_t channel;

    int startPos;
    int endPos;

    int currentPos;

    Servo (uint8_t pin, uint8_t channel, uint8_t inverted);
    // Set the pulse width directly
    void setPulseWidth (int width);
    // Set a position between 0 and 1000
    void setPos (int pos);
    void moveStart ();
    void moveEnd ();
};

#endif