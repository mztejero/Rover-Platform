#ifndef MOTOR_H
#define MOTOR_H

#include "freertos/FreeRTOS.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_gen.h"

void setup_motors(void);
void duty_cycle(int duty_1, int duty_2, int duty_3, int duty_4);

#endif