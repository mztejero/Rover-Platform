#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"

void setup_mag_i2c(void);
void read_magnetometer(void *arg);
void mag_task(void *arg);

#endif