#ifndef SPEED_CONTROLLER_H
#define SPEED_CONTROLLER_H

#include "motor.h"
#include "encoder.h"

#include <stdbool.h>
#include <stdio.h>

void set_motor_speed(double duty_1, double duty_2, double duty_3, double duty_4);

#endif