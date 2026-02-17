#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"

typedef struct {
    gpio_num_t sda;
    gpio_num_t scl;
    gpio_num_t enc1_1;
    gpio_num_t enc2_1;
    gpio_num_t enc1_2;
    gpio_num_t enc2_2;
    gpio_num_t enc1_3;
    gpio_num_t enc2_3;
    gpio_num_t enc1_4;
    gpio_num_t enc2_4;
    gpio_num_t pwm_1;
    gpio_num_t pwm_2;
    gpio_num_t pwm_3;
    gpio_num_t pwm_4;
    gpio_num_t ain1_1;
    gpio_num_t ain2_1;
    gpio_num_t bin1_1;
    gpio_num_t bin2_1;
    gpio_num_t ain1_2;
    gpio_num_t ain2_2;
    gpio_num_t bin1_2;
    gpio_num_t bin2_2;
    gpio_num_t stby;
} PinConfig;

extern const PinConfig pins;

extern i2c_master_bus_handle_t bus_handle;

void setup_gpio(void);
void setup_i2c(void);

#endif