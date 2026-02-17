#include "config.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"

const PinConfig pins = {
    .sda = GPIO_NUM_8,
    .scl = GPIO_NUM_9,
    .enc1_1 = GPIO_NUM_5,
    .enc2_1 = GPIO_NUM_4,
    .enc1_2 = GPIO_NUM_6,
    .enc2_2 = GPIO_NUM_7,
    .enc1_3 = GPIO_NUM_15,
    .enc2_3 = GPIO_NUM_16,
    .enc1_4 = GPIO_NUM_18,
    .enc2_4 = GPIO_NUM_17,
    .pwm_1 = GPIO_NUM_1,
    .pwm_2 = GPIO_NUM_48,
    .pwm_3 = GPIO_NUM_21,
    .pwm_4 = GPIO_NUM_10,
    .ain1_1 = GPIO_NUM_40,
    .ain2_1 = GPIO_NUM_2,
    .bin1_1 = GPIO_NUM_39,
    .bin2_1 = GPIO_NUM_38,
    .ain1_2 = GPIO_NUM_14,
    .ain2_2 = GPIO_NUM_13,
    .bin1_2 = GPIO_NUM_11,
    .bin2_2 = GPIO_NUM_12,
    .stby = GPIO_NUM_47
};

void setup_gpio(void) {    
    // Encoder Terminals
    gpio_config_t conf_enc = {
        .pin_bit_mask = (1ULL << pins.enc1_1) |
                        (1ULL << pins.enc2_1) |
                        (1ULL << pins.enc1_2) |
                        (1ULL << pins.enc2_2) |
                        (1ULL << pins.enc1_3) |
                        (1ULL << pins.enc2_3) |
                        (1ULL << pins.enc1_4) |
                        (1ULL << pins.enc2_4),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&conf_enc);

    // Digital Outputs
    gpio_config_t conf_digital_outputs = {
        .pin_bit_mask = (1ULL << pins.pwm_1) |
                        (1ULL << pins.pwm_2) |
                        (1ULL << pins.pwm_3) |
                        (1ULL << pins.pwm_4) |
                        (1ULL << pins.ain1_1) |
                        (1ULL << pins.ain2_1) |
                        (1ULL << pins.bin1_1) |
                        (1ULL << pins.bin2_1) |
                        (1ULL << pins.ain1_2) |
                        (1ULL << pins.ain2_2) |
                        (1ULL << pins.bin1_2) |
                        (1ULL << pins.bin2_2) |
                        (1ULL << pins.stby),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&conf_digital_outputs);
}

i2c_master_bus_handle_t bus_handle;
void setup_i2c(void) {
        i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = pins.scl,
        .sda_io_num = pins.sda,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}