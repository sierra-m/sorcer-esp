#ifndef MICRO_SERVO_SG90_H
#define MICRO_SERVO_SG90_H

#include "Servo.h"

#define SG90_PULSE_WIDTH_MIN 350
#define SG90_PULSE_WIDTH_MAX 2050
#define SG90_INVERTED 0
#define SG90_FULL_MOVE_DELAY_MS 200

class MicroServoSG90: public Servo {
  public:
    MicroServoSG90 (uint8_t pin, uint8_t channel)
    : Servo(SG90_PULSE_WIDTH_MIN, SG90_PULSE_WIDTH_MAX, SG90_FULL_MOVE_DELAY_MS, pin, channel, SG90_INVERTED) {}
};

#endif