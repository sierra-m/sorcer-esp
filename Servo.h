#ifndef SERVO_H
#define SERVO_H

#include <ctype.h>
#include "Arduino.h"

// Bit resolution for PWM duty cycle
#define SERVO_MAX_BIT_NUM 14

class Servo {
  public:
    // GPIO pin to bind to
    uint8_t pin;
    // PWM channel
    uint8_t channel;

    // Pulse width for "start" position, aka most CCW position. Represented as position 0
    int startPulseWidth;
    // Pulse width for "end" position, aka most CW position. Represented as position 1000
    int endPulseWidth;

    // Movement delay of full rotation
    int fullMoveDelay;

    Servo (uint16_t minPulseWidth, uint16_t maxPulseWidth, int fullMoveDelay, uint8_t pin, uint8_t channel, uint8_t inverted);
    // Set the pulse width directly
    void setPulseWidth (int width);
    // Set a position between 0 and 1000
    void setPos (int pos, uint8_t blocking = 0);
    // Move to start position
    void moveStart (uint8_t blocking = 0);
    // Move to end position
    void moveEnd (uint8_t blocking = 0);
    // Invert start and end directions
    void invert ();
    // Calculate movement delay
    int calcDelay (int newPos);
  protected:
    // Current position on range [0, 1000]
    int currentPos;

    // Calculate approximate movement delay
    void waitMovement (int newPos);
};

#endif