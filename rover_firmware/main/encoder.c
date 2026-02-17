#include "config.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "driver/gptimer_types.h"

#define TIMER_FREQ 1000000 // Hz
#define RESOLUTION 576 // PPR
#define PI 3.14159
#define WHEEL_RADIUS 7e-2 // m

// Global Variables
volatile int count_1;
volatile int count_2;
volatile int count_3;
volatile int count_4;

// Define Handles
QueueHandle_t queue_1;
QueueHandle_t queue_2;
QueueHandle_t queue_3;
QueueHandle_t queue_4;

// Encoder Lookup Table
int8_t enc[256] = {0};
void enc_table(void) {
    enc[0b0001] = -1;
    enc[0b0111] = -1;
    enc[0b1110] = -1;
    enc[0b1000] = -1;

    enc[0b0010] = 1;
    enc[0b0100] = 1;
    enc[0b1101] = 1;
    enc[0b1011] = 1;
}

// ISR's
void IRAM_ATTR enc_1_isr_handler(void *arg) {
    static uint8_t state_1 = 0b1111;

    uint8_t enc1 = gpio_get_level(pins.enc1_1);
    uint8_t enc2 = gpio_get_level(pins.enc2_1);

    uint8_t curr = (enc1 << 1) | enc2;
    state_1 = (state_1 << 2) | curr;
    uint8_t state = state_1;

    BaseType_t xHPW = pdFALSE;
    xQueueSendFromISR(queue_1, &state, &xHPW);
    if (xHPW) portYIELD_FROM_ISR();
}

void IRAM_ATTR enc_2_isr_handler(void *arg) {
    static uint8_t state_2 = 0b1111;

    uint8_t enc1 = gpio_get_level(pins.enc1_2);
    uint8_t enc2 = gpio_get_level(pins.enc2_2);
    
    uint8_t curr = (enc1 << 1) | enc2;
    state_2 = (state_2 << 2) | curr;
    uint8_t state = state_2;

    BaseType_t xHPW = pdFALSE;
    xQueueSendFromISR(queue_2, &state, &xHPW);
    if (xHPW) portYIELD_FROM_ISR(); 
}

void IRAM_ATTR enc_3_isr_handler(void *arg) {
    static uint8_t state_3 = 0b1111;

    uint8_t enc1 = gpio_get_level(pins.enc1_3);
    uint8_t enc2 = gpio_get_level(pins.enc2_3);
    
    uint8_t curr = (enc1 << 1) | enc2;
    state_3 = (state_3 << 2) | curr;
    uint8_t state = state_3;

    BaseType_t xHPW = pdFALSE;
    xQueueSendFromISR(queue_3, &state, &xHPW);
    if (xHPW) portYIELD_FROM_ISR(); 
}

void IRAM_ATTR enc_4_isr_handler(void *arg) {
    static uint8_t state_4 = 0b1111;

    uint8_t enc1 = gpio_get_level(pins.enc1_4);
    uint8_t enc2 = gpio_get_level(pins.enc2_4);
    
    uint8_t curr = (enc1 << 1) | enc2;
    state_4 = (state_4 << 2) | curr;
    uint8_t state = state_4;

    BaseType_t xHPW = pdFALSE;
    xQueueSendFromISR(queue_4, &state, &xHPW);
    if (xHPW) portYIELD_FROM_ISR(); 
}

// void state_reader(uint8_t state) {
//     char binary_string[9];
//     for (int i = 7; i >= 0; --i) {
//         if ((state >> i) & 1) {
//             binary_string[7 - i] = '1';
//         }
//         else {
//             binary_string[7 - i] = '0';
//         }
//     }
//     binary_string[8] = '\0';
//     printf("binary: 0b%s\n", binary_string);
// }

void enc_1_isr_task(void *arg) {
    uint8_t state;
    while (1) {
        if (xQueueReceive(queue_1, &state, portMAX_DELAY) == pdTRUE) {
            count_1 += enc[state & 0x0F];
            // printf("count 1: %d\n", count_1);
        }
    }
}

void enc_2_isr_task(void *arg) {
    uint8_t state;
    while (1) {
        if (xQueueReceive(queue_2, &state, portMAX_DELAY) == pdTRUE) {
            count_2 += enc[state & 0x0F];
        }
    }
}

void enc_3_isr_task(void *arg) {
    uint8_t state;
    while (1) {
        if (xQueueReceive(queue_3, &state, portMAX_DELAY) == pdTRUE) {
            count_3 += enc[state & 0x0F];
        }
    }
}

void enc_4_isr_task(void *arg) {
    uint8_t state;
    while (1) {
        if (xQueueReceive(queue_4, &state, portMAX_DELAY) == pdTRUE) {
            count_4 += enc[state & 0x0F];
        }
    }
}

// Queue Setup
void setup_queues(void) {
    queue_1 = xQueueCreate(128, sizeof(uint8_t));
    queue_2 = xQueueCreate(128, sizeof(uint8_t));
    queue_3 = xQueueCreate(128, sizeof(uint8_t));
    queue_4 = xQueueCreate(128, sizeof(uint8_t));
    if (!queue_1 || !queue_2 || !queue_3 || !queue_4) {
        printf("Failed to create queues\n");
    }

    if (xTaskCreatePinnedToCore(enc_1_isr_task, "enc_1_isr_task", 4096, NULL, 2, NULL, 1) != pdPASS) {
        printf("Failed to create enc_1_isr_task\n");
    }
    if (xTaskCreatePinnedToCore(enc_2_isr_task, "enc_2_isr_task", 4096, NULL, 2, NULL, 1) != pdPASS) {
        printf("Failed to create enc_2_isr_task\n");
    }
    if (xTaskCreatePinnedToCore(enc_3_isr_task, "enc_3_isr_task", 4096, NULL, 2, NULL, 1) != pdPASS) {
        printf("Failed to create enc_3_isr_task\n");
    }
    if (xTaskCreatePinnedToCore(enc_4_isr_task, "enc_4_isr_task", 4096, NULL, 2, NULL, 1) != pdPASS) {
        printf("Failed to create enc_4_isr_task\n");
    }
}

// Timer
gptimer_handle_t gptimer;

gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_APB,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = TIMER_FREQ,
};

gptimer_alarm_config_t alarm_config = {
    .alarm_count = 100000, // ticks to count to
    .reload_count = 0, // reset tick value
    .flags.auto_reload_on_alarm = true, // reset timer
};

uint64_t count;
int count_1_prev;
int count_2_prev;
int count_3_prev;
int count_4_prev;

double time;

double ds1;
double ds2;
double ds3;
double ds4;

double v1;
double v2;
double v3;
double v4;

static bool reset_count(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    // function to call on alarm
    time = (double)count / TIMER_FREQ;

    ds1 = (count_1 - count_1_prev) * 2 * PI * WHEEL_RADIUS / RESOLUTION;
    ds2 = (count_2 - count_2_prev) * 2 * PI * WHEEL_RADIUS / RESOLUTION;
    ds3 = (count_3 - count_3_prev) * 2 * PI * WHEEL_RADIUS / RESOLUTION;
    ds4 = (count_4 - count_4_prev) * 2 * PI * WHEEL_RADIUS / RESOLUTION;

    v1 = ds1 / time;
    v2 = ds2 / time;
    v3 = ds3 / time;
    v4 = ds4 / time;

    count_1_prev = count_1;
    count_2_prev = count_2;
    count_3_prev = count_3;
    count_4_prev = count_4;


    return false;
}

gptimer_event_callbacks_t gptimer_callback = {
    .on_alarm = reset_count, // Call the user callback function when the alarm event occurs
};

void create_enc_timer(void) {
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &gptimer_callback, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void get_count(void) {
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));
    // printf("COUNT: %d\n", count_1);
    // printf("v1: %f m/s, v2: %f m/s, v3: %f m/s, v4: %f m/s\n", v1, v2, v3, v4);
    // printf("count 1: %d, count 2: %d, count 3: %d, count 4: %d\n", count_1, count_2, count_3, count_4);
}

// ISR Setup
void setup_isr(void) {
    gpio_install_isr_service(0);

    gpio_isr_handler_add(pins.enc1_1, enc_1_isr_handler, (void* )pins.enc1_1);
    gpio_isr_handler_add(pins.enc2_1, enc_1_isr_handler, (void* )pins.enc2_1);
    gpio_isr_handler_add(pins.enc1_2, enc_2_isr_handler, (void* )pins.enc1_2);
    gpio_isr_handler_add(pins.enc2_2, enc_2_isr_handler, (void* )pins.enc2_2);
    gpio_isr_handler_add(pins.enc1_3, enc_3_isr_handler, (void* )pins.enc1_3);
    gpio_isr_handler_add(pins.enc2_3, enc_3_isr_handler, (void* )pins.enc2_3);
    gpio_isr_handler_add(pins.enc1_4, enc_4_isr_handler, (void* )pins.enc1_4);
    gpio_isr_handler_add(pins.enc2_4, enc_4_isr_handler, (void* )pins.enc2_4);
}