#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "math.h"

#include "config.h"

#define DEVICE_ADDRESS 0x0D
#define SCL_SPEED 100000
#define DATA_LENGTH 7

#define LSB_PER_G 1 // 12000

#define alpha_a 0.5
#define alpha_w 0.5

uint8_t mag_reg = 0x00;

uint8_t control_reg = 0x09;
uint8_t control_reg_2 = 0x0A;
uint8_t set_reset_reg = 0x0B;

i2c_master_dev_handle_t mag_dev_handle;

void setup_mag_i2c(void) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DEVICE_ADDRESS,
        .scl_speed_hz = SCL_SPEED
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &mag_dev_handle));

    uint8_t control_reg_2_msg[2] = { control_reg_2, 0b11000000};
    i2c_master_transmit(mag_dev_handle, control_reg_2_msg, sizeof(control_reg_2_msg), 100);

    uint8_t set_reset_msg[2] = {set_reset_reg, 0x01};
    ESP_ERROR_CHECK(i2c_master_transmit(mag_dev_handle, set_reset_msg, sizeof(set_reset_msg), 100));

    uint8_t control_reg_msg[2] = {control_reg, 0b01000001};
    ESP_ERROR_CHECK(i2c_master_transmit(mag_dev_handle, control_reg_msg, sizeof(control_reg_msg), 100));
}

void read_magnetometer(void *arg) {
    uint8_t mag_raw[6];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(mag_dev_handle, &mag_reg, 1, mag_raw, sizeof(mag_raw), 100));
    int16_t x_raw = ((mag_raw[1] << 8) | mag_raw[0]);
    int16_t y_raw = ((mag_raw[3] << 8) | mag_raw[2]);
    int16_t z_raw = ((mag_raw[5] << 8) | mag_raw[4]);

    float x = (float)x_raw / LSB_PER_G;
    float y = (float)y_raw / LSB_PER_G;
    float z = (float)z_raw / LSB_PER_G;

    float yaw = atan2(y, x) * 180/3.14159;

    // imu->ax = (1-alpha_a) * imu->ax + alpha_a * ax;
    // imu->ay = (1-alpha_a) * imu->ay + alpha_a * ay;
    // imu->az = (1-alpha_a) * imu->az + alpha_a * az;

    // printf("yaw:%f:x:%f:y:%f:z:%f\n", yaw, x, y, z);
}

void mag_task(void *arg) {
    while (1) {
        read_magnetometer(NULL);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}