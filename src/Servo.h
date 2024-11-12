#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include "Arduino.h"

// Bit resolution for PWM duty cycle
#define SERVO_MAX_BIT_NUM 14
// Mulitplier used to calc the max delay in lower-speed move operatoin, where
// maxDelay == (minDelay * MAX_DELAY_MULT).
// The max delay should be < 16383, which is the max supported value for delayMicroseconds().
// Min delay is calculated from fullMoveDelay
#define SERVO_MAX_DELAY_MULT 5

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

    // Min and max delays used to implement max and min speeds, respectively
    int minIncrDelayMicros;
    int maxIncrDelayMicros;

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
    // Get current pos
    int getPos ();
    // Set the current speed
    void setSpeed (uint8_t newSpeed);
    // Get the current speed
    uint8_t getSpeed ();
    // Indicates that position needs updating
    uint8_t requiresUpdate ();
    // Update servo position (for async lower-speed moves)
    void update ();
  protected:
    // Current position on range [0, 1000]
    int currentPos;

    // Tracks speed to drive movement changes by on range [0,100]
    uint8_t speed;
    // Tracks single increment delay (aka 1/speed)
    int incrementDelay;

    // Tracks next desired pulse width for async moves
    int nextPos;

    // Tracks the last time recorded for async events
    unsigned long lastTimeMicros;

    // Calculate approximate movement delay
    void waitMovement (int newPos);
};

#endif