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

#include "config.h"

#define DEVICE_ADDRESS 0b1101010
#define SCL_SPEED 100000
#define DATA_LENGTH 7

#define LSB_PER_G 16384 // ±2g 8192 for ±4g
#define LSB_PER_DEG 131.0 // ±250dps 65.6 for ±500dps
#define g 9.81

#define acc_offset_x 0.00
#define acc_offset_y 0.00
#define acc_offset_z 0.31

#define alpha_a 0.5
#define alpha_w 0.5

// datasheet: https://www.mouser.com/pdfDocs/STM_ISM330DHCX_DS.pdf?srsltid=AfmBOoqZ5He-GGR3J2Aztb9T3-6cBYvTeMTWk-NMwgHzJIzqYs8aOSO2&utm_source=chatgpt.com#page=70&zoom=100,81,252

uint8_t acc_reg = 0x28;
uint8_t gyro_reg = 0x22;

// uint8_t power_reg = 0x6B;
uint8_t acc_conf_reg = 0x10;
uint8_t gyro_conf_reg = 0x11;

float ax;
float ay;
float az;

float wx;
float wy;
float wz;

// acc high performance mode is enabled and can be disabled by setting XL_HM_MODE to 1 in CTRL6_C
// gyro high performance mode is enabled and can be disabled by setting G_HM_MODE to 1 in CTRL7_G

uint8_t data_rd;

typedef struct {
    float ax;
    float ay;
    float az;
    float wx;
    float wy;
    float wz;
} imu_data;

// imu_data imu = {
//     .ax = 0,
//     .ay = 0,
//     .az = 0,
//     .wx = 0,
//     .wy = 0,
// };

i2c_master_dev_handle_t imu_dev_handle;

void setup_imu_i2c(void) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DEVICE_ADDRESS,
        .scl_speed_hz = SCL_SPEED
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &imu_dev_handle));

    // uint8_t wake_up[2] = {power_reg, 0x00};
    // ESP_ERROR_CHECK(i2c_master_transmit(imu_dev_handle, wake_up, sizeof(wake_up), 100));

    uint8_t gyro_conf[2] = {gyro_conf_reg, 0b00110000};
    ESP_ERROR_CHECK(i2c_master_transmit(imu_dev_handle, gyro_conf, sizeof(gyro_conf), 100));

    uint8_t acc_conf[2] = {acc_conf_reg, 0b00110000};
    ESP_ERROR_CHECK(i2c_master_transmit(imu_dev_handle, acc_conf, sizeof(acc_conf), 100));


}

void read_imu(void *arg) {
    uint8_t acc_raw[6];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(imu_dev_handle, &acc_reg, 1, acc_raw, sizeof(acc_raw), 100));
    int16_t ax_raw = ((acc_raw[1] << 8) | acc_raw[0]);
    int16_t ay_raw = ((acc_raw[3] << 8) | acc_raw[2]);
    int16_t az_raw = ((acc_raw[5] << 8) | acc_raw[4]);

    ax = (float)ax_raw * g / LSB_PER_G;
    ay = (float)ay_raw * g / LSB_PER_G;
    az = (float)az_raw * g / LSB_PER_G;

    az -= acc_offset_z;

    // imu->ax = (1-alpha_a) * imu->ax + alpha_a * ax;
    // imu->ay = (1-alpha_a) * imu->ay + alpha_a * ay;
    // imu->az = (1-alpha_a) * imu->az + alpha_a * az;

    uint8_t gyro_raw[6];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(imu_dev_handle, &gyro_reg, 1, gyro_raw, sizeof(gyro_raw), 100));
    int16_t wx_raw = ((gyro_raw[1] << 8) | gyro_raw[0]);
    int16_t wy_raw = ((gyro_raw[3] << 8) | gyro_raw[2]);
    int16_t wz_raw = ((gyro_raw[5] << 8) | gyro_raw[4]);

    wx = (float)wx_raw / LSB_PER_DEG;
    wy = (float)wy_raw / LSB_PER_DEG;
    wz = (float)wz_raw / LSB_PER_DEG;

    // imu->wx = (1-alpha_w) * imu->wx + alpha_w * wx;
    // imu->wy = (1-alpha_w) * imu->wy + alpha_w * wy;
    // imu->wz = (1-alpha_w) * imu->wz + alpha_w * wz;

    // printf(":ax:%f:ay:%f:az:%f", ax, ay, az);
    // printf(":wx:%f:wy:%f:wz:%f:\n", wx, wy, wz);
}

void imu_task(void *arg) {
    while (1) {
        read_imu(NULL);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}