#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

extern double v1_ref;
extern double v2_ref;
extern double v3_ref;
extern double v4_ref;

void setup_uart(void);
void uart_read_task(void *arg);
void uart_write_task(void *arg);

#endif