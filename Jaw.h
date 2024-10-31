#ifndef JAW_H
#define JAW_H

#include <stdint.h>
#include "Arduino.h"
#include "MicroServoSG90.h"

class Jaw {
  public:
    MicroServoSG90 *jawServo;

    Jaw (MicroServoSG90 *jawServo);
    // Open mouth
    void open (uint8_t blocking = 0);
    // Close mouth
    void close (uint8_t blocking = 0);
    // Move jaw up and down rapidly
    void laugh (int count = 3);
};

#endif