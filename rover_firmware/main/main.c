// idf.py -p /dev/cu.usbserial-0001 -b 115200 flash monitor
#include "config.h"
#include "imu.h"
#include "magnetometer.h"
#include "motor.h"
#include "encoder.h"
#include "speed_controller.h"
#include "uart_interface.h"

void app_main (void) {
    setup_gpio();

    vTaskDelay(pdMS_TO_TICKS(50));

    setup_i2c();
    setup_imu_i2c();
    // setup_mag_i2c();
    setup_uart();

    xTaskCreatePinnedToCore(imu_task, "imu_task", 8192, NULL, 4, NULL, 0);
    // xTaskCreatePinnedToCore(mag_task, "mag_task", 8192, NULL, 4, NULL, 0);
    xTaskCreate(uart_read_task, "uart_task", 8192, NULL, 4, NULL);
    xTaskCreate(uart_write_task, "uart_task", 8192, NULL, 4, NULL);

    enc_table();
    setup_queues();
    setup_isr();
    create_enc_timer();

    setup_motors();

    while(1) {
        set_motor_speed(v1_ref, v2_ref, v3_ref, v4_ref);
        vTaskDelay(pdMS_TO_TICKS(10));
        get_count();
    }
}

// idf.py -p /dev/cu.usbmodem14101 -b 115200 flash monitor
// idf.py -p /dev/cu.usbmodem14101 -b 115200 monitor

// idf.py -p /dev/cu.usbserial-0001 -b 115200 flash monitor
// idf.py -p /dev/cu.usbserial-0001 -b 115200 monitor

// CUSTOM PCB

// idf.py -p /dev/cu.usbserial-1410 -b 115200 flash monitor
// idf.py -p /dev/cu.usbserial-1410 -b 115200 monitor