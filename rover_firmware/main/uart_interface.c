#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "encoder.h"
#include "imu.h"

#define UART_PORT_NUM UART_NUM_0
#define uart_buffer_size (1024 * 2)

double tt = 0;
double tt_prev = 0;
double dt = 0;

double v1_ref;
double v2_ref;
double v3_ref;
double v4_ref;

void setup_uart(void) {

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, uart_buffer_size, uart_buffer_size, 0, NULL, 0));

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
}

void parse_line(char *line, int len) {    
    int v1_pos = 0;
    int v2_pos = 0;    
    int v3_pos = 0;
    int v4_pos = 0;

    char v1_str[64];
    char v2_str[64];
    char v3_str[64];
    char v4_str[64];

    enum state {
        V1_STATE,
        V2_STATE,
        V3_STATE,
        V4_STATE
    };

    enum state v_state = V1_STATE;

    for (int i = 0; i < len; i++) {
        char c = line[i];
        switch(v_state) {
            case V1_STATE:
            if (c == 'v') {
                continue;
            }
            else if (c == ':') {
                v1_str[v1_pos] = '\0';
                v1_pos = 0;
                v_state = V2_STATE;
            }
            else {
                v1_str[v1_pos++] = c;
            }
            break;
            
            case V2_STATE:
            if (c == ':') {
                v2_str[v2_pos] = '\0';
                v2_pos = 0;
                v_state = V3_STATE;
            }
            else {
                v2_str[v2_pos++] = c;
            }
            break;

            case V3_STATE:
            if (c == ':') {
                v3_str[v3_pos] = 0;
                v3_pos = 0;
                v_state = V4_STATE;
            }
            else {
                v3_str[v3_pos++] = c;
            }
            break;

            case V4_STATE:
            if (c == '\n') {
                v4_str[v4_pos] = '\0';
                v4_pos = 0;
                v_state = V1_STATE;

                v1_ref = strtod(v1_str, NULL);
                v2_ref = strtod(v2_str, NULL);
                v3_ref = strtod(v3_str, NULL);
                v4_ref = strtod(v4_str, NULL);

                // printf("v1: %f, v2: %f, v3: %f, v4: %f,          dt: %f\n", v1_ref, v2_ref, v3_ref, v4_ref, (float)dt / configTICK_RATE_HZ);
            }
            else {
                v4_str[v4_pos++] = c;
            }
            break;

        }
    }
}

void uart_read_task(void *arg) {
    uint8_t buf[64];
    char line[64];

    while (1) {
        tt = xTaskGetTickCount();
        dt = tt - tt_prev;
        int len = uart_read_bytes(UART_PORT_NUM, buf, sizeof(buf), pdMS_TO_TICKS(30));
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                char c = buf[i];
                line[i] = c;
                if (c == '\n') {
                    line[i] = c;
                    line[i + 1] = '\0';
                    if (line[0] == 'v') {
                        // printf("line: %s,               dt: %f, i: %d\n", line, (float)dt / configTICK_RATE_HZ, i);
                        parse_line(line, i + 1);
                    }
                }
            }
        }
        // ESP_ERROR_CHECK(uart_flush(UART_PORT_NUM));
        tt_prev = tt;
    }    
}

void uart_write_task(void *arg) {
    char data_str[254];
    // char d_str[64];
    // char v_str[64];
    // char a_str[64];
    // char g_str[64];

    while(1) {
        int len_data_str = snprintf(data_str,
                                    sizeof(data_str),
                                    "d:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f\n",
                                    ds1, ds2, ds3, ds4,
                                    v1, v2, v3, v4,
                                    ax, ay, az,
                                    wx, wy, wz
                                );
        if (len_data_str > 0 && len_data_str < sizeof(data_str)) {
            uart_write_bytes(UART_PORT_NUM, data_str, len_data_str);
        }

        // int len_d_str = snprintf(d_str, sizeof(d_str), "d:%f:%f:%f:%f\n", ds1, ds2, ds3, ds4);
        // if (len_d_str > 0 && len_d_str < sizeof(d_str)) {
        //     uart_write_bytes(UART_PORT_NUM, d_str, len_d_str);
        // }

        // int len_v_str = snprintf(v_str, sizeof(v_str), "v:%f:%f:%f:%f\n", v1, v2, v3, v4);
        // if (len_v_str > 0 && len_v_str < sizeof(v_str)) {
        //     uart_write_bytes(UART_PORT_NUM, v_str, len_v_str);
        // }

        // int len_a_str = snprintf(a_str, sizeof(a_str), "a:%f:%f:%f\n", ax, ay, az);
        // if (len_a_str > 0 && len_a_str < sizeof(a_str)) {
        //     uart_write_bytes(UART_PORT_NUM, a_str, len_a_str);
        // }

        // int len_g_str = snprintf(g_str, sizeof(g_str), "w:%f:%f:%f\n", wx, wy, wz);
        // if (len_g_str > 0 && len_g_str < sizeof(g_str)) {
        //     uart_write_bytes(UART_PORT_NUM, g_str, len_g_str);
        // }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}