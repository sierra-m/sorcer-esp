#ifndef SERVO_DS3218_H
#define SERVO_DS3218_H

#include "Servo.h"

#define DS3218_PULSE_WIDTH_MIN 350
#define DS3218_PULSE_WIDTH_MAX 2150
#define DS3218_INVERTED 0
#define DS3218_FULL_MOVE_DELAY_MS 1800

class ServoDS3218: public Servo {
  public:
    ServoDS3218 (uint8_t pin, uint8_t channel)
    : Servo(DS3218_PULSE_WIDTH_MIN, DS3218_PULSE_WIDTH_MAX, DS3218_FULL_MOVE_DELAY_MS, pin, channel, DS3218_INVERTED) {}
};

#endif