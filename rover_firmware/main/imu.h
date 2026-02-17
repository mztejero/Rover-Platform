#ifndef IMU_H
#define IMU_H

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"

extern float ax;
extern float ay;
extern float az;

extern float wx;
extern float wy;
extern float wz;

void setup_imu_i2c(void);
void read_imu(void *arg);
void imu_task(void *arg);

#endif