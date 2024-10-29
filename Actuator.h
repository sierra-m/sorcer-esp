#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "ServoDS3218.h"

typedef enum {
  CLOCKWISE,
  COUNTER_CLOCKWISE
} ExtendDirection;

class Actuator {
  public:
    // Servo pointers
    ServoDS3218 *leftServo;
    ServoDS3218 *rightServo;

    Actuator (ServoDS3218 *leftServo, ServoDS3218 *rightServo, ExtendDirection leftServoDir = CLOCKWISE);
    // Fully retract arms
    void retract (uint8_t blocking = 0);
    // Fully extend arms
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
  protected:
    // Calculate and wait max delay. Used for blocking calls with both servos moving
    void waitMaxDelay (int leftNewPos, int rightNewPos);
};

#endif