#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "Arduino.h"
#include "ServoDS3218.h"

typedef enum {
  CLOCKWISE,
  COUNTER_CLOCKWISE
} ExtendDirection;

// Defines movement control commands for a 2-axis linear actuator gimbal
// moving an animatronic head
class Actuator {
  public:
    // Servo pointers
    ServoDS3218 *leftServo;
    ServoDS3218 *rightServo;

    Actuator (ServoDS3218 *leftServo, ServoDS3218 *rightServo, ExtendDirection leftServoDir = CLOCKWISE);
    // Handles movement of both servos
    void moveBoth (int leftNewPos, int rightNewPos, uint8_t blocking = 0);
    // Set speed for both servos
    void setSpeed (uint8_t newSpeed);
    // Move arms to initial state (middle)
    void reset ();
    // Fully retract arms
    void retract (uint8_t blocking = 0);
    // Fully extend arms (does not extend to unloading position)
    void extend (uint8_t blocking = 0);
    // Move to gimbal unloading position
    void unload (uint8_t blocking = 0);
    // Extend arms to halfway position
    void extendHalf (uint8_t blocking = 0);
    // Tilt gimbal to the right, where amount defines total tilt on range [0, 1000]
    void tiltRight (int amount = 1000, uint8_t blocking = 0);
    // Tilt gimbal to the left, where amount defines total tilt on range [0, 1000]
    void tiltLeft (int amount = 1000, uint8_t blocking = 0);
    // Bounce the gimbal up and down
    void bounce (uint8_t blocking = 0);
    // Shake the gimbal slightly back and forth
    void shake (int count = 1);
  protected:
    // Calculate max delay. Used for blocking calls with both servos moving
    int calcMaxDelay (int leftNewPos, int rightNewPos);
};

#endif