#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_gen.h"
// #include "driver/mcpwm_prelude.h" 

#define PWM_FREQ 80e6
#define PWM_TICKS 8000

// Handles
static mcpwm_timer_handle_t pwm_12_timer_handle = NULL;
static mcpwm_timer_handle_t pwm_34_timer_handle = NULL;


static mcpwm_oper_handle_t pwm_12_oper_handle = NULL;
static mcpwm_oper_handle_t pwm_34_oper_handle = NULL;

static mcpwm_cmpr_handle_t pwm_1_cmpr_handle = NULL;
static mcpwm_cmpr_handle_t pwm_2_cmpr_handle = NULL;
static mcpwm_cmpr_handle_t pwm_3_cmpr_handle = NULL;
static mcpwm_cmpr_handle_t pwm_4_cmpr_handle = NULL;

static mcpwm_gen_handle_t pwm_1_gen_handle = NULL;
static mcpwm_gen_handle_t pwm_2_gen_handle = NULL;
static mcpwm_gen_handle_t pwm_3_gen_handle = NULL;
static mcpwm_gen_handle_t pwm_4_gen_handle = NULL;

void setup_motors(void) {
    // TIMER
    mcpwm_timer_config_t pwm_12_timer_config = {
        .group_id = 0,
        .intr_priority = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_PLL160M,
        .resolution_hz = PWM_FREQ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = PWM_TICKS
    };
    mcpwm_timer_config_t pwm_34_timer_config = {
        .group_id = 1,
        .intr_priority = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_PLL160M,
        .resolution_hz = PWM_FREQ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = PWM_TICKS
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&pwm_12_timer_config, &pwm_12_timer_handle));
    ESP_ERROR_CHECK(mcpwm_new_timer(&pwm_34_timer_config, &pwm_34_timer_handle));

    // OPERATOR
    mcpwm_operator_config_t pwm_12_operator_config = {
        .group_id = 0,
        .intr_priority = 0
    };
    mcpwm_operator_config_t pwm_34_operator_config = {
        .group_id = 1,
        .intr_priority = 0
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(&pwm_12_operator_config, &pwm_12_oper_handle));
    ESP_ERROR_CHECK(mcpwm_new_operator(&pwm_34_operator_config, &pwm_34_oper_handle));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(pwm_12_oper_handle, pwm_12_timer_handle));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(pwm_34_oper_handle, pwm_34_timer_handle));

    // COMPARATOR
    mcpwm_comparator_config_t pwm_1_comparator_config = {
        .intr_priority = 0
    };
    mcpwm_comparator_config_t pwm_2_comparator_config = {
        .intr_priority = 0
    };
    mcpwm_comparator_config_t pwm_3_comparator_config = {
        .intr_priority = 0
    };
    mcpwm_comparator_config_t pwm_4_comparator_config = {
        .intr_priority = 0
    };
    
    ESP_ERROR_CHECK(mcpwm_new_comparator(pwm_12_oper_handle, &pwm_1_comparator_config, &pwm_1_cmpr_handle));
    ESP_ERROR_CHECK(mcpwm_new_comparator(pwm_12_oper_handle, &pwm_2_comparator_config, &pwm_2_cmpr_handle));
    ESP_ERROR_CHECK(mcpwm_new_comparator(pwm_34_oper_handle, &pwm_3_comparator_config, &pwm_3_cmpr_handle));
    ESP_ERROR_CHECK(mcpwm_new_comparator(pwm_34_oper_handle, &pwm_4_comparator_config, &pwm_4_cmpr_handle));

    // GENERATOR
    mcpwm_generator_config_t pwm_1_generator_config = {
        .gen_gpio_num = pins.pwm_1
    };
    mcpwm_generator_config_t pwm_2_generator_config = {
        .gen_gpio_num = pins.pwm_2
    };
    mcpwm_generator_config_t pwm_3_generator_config = {
        .gen_gpio_num = pins.pwm_3
    };
    mcpwm_generator_config_t pwm_4_generator_config = {
        .gen_gpio_num = pins.pwm_4
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(pwm_12_oper_handle, &pwm_1_generator_config, &pwm_1_gen_handle));
    ESP_ERROR_CHECK(mcpwm_new_generator(pwm_12_oper_handle, &pwm_2_generator_config, &pwm_2_gen_handle));
    ESP_ERROR_CHECK(mcpwm_new_generator(pwm_34_oper_handle, &pwm_3_generator_config, &pwm_3_gen_handle));
    ESP_ERROR_CHECK(mcpwm_new_generator(pwm_34_oper_handle, &pwm_4_generator_config, &pwm_4_gen_handle));

    // EVENTS
    // Timer event
    mcpwm_gen_timer_event_action_t pwm_12_timer_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH
    };
    mcpwm_gen_timer_event_action_t pwm_34_timer_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH
    };

    // Compare events
    mcpwm_gen_compare_event_action_t pwm_1_compare_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = pwm_1_cmpr_handle,
        .action = MCPWM_GEN_ACTION_LOW
    };
    mcpwm_gen_compare_event_action_t pwm_2_compare_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = pwm_2_cmpr_handle,
        .action = MCPWM_GEN_ACTION_LOW
    };
    mcpwm_gen_compare_event_action_t pwm_3_compare_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = pwm_3_cmpr_handle,
        .action = MCPWM_GEN_ACTION_LOW
    };
    mcpwm_gen_compare_event_action_t pwm_4_compare_event = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = pwm_4_cmpr_handle,
        .action = MCPWM_GEN_ACTION_LOW
    };

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(pwm_1_gen_handle, pwm_12_timer_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(pwm_2_gen_handle, pwm_12_timer_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(pwm_3_gen_handle, pwm_34_timer_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(pwm_4_gen_handle, pwm_34_timer_event));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(pwm_1_gen_handle, pwm_1_compare_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(pwm_2_gen_handle, pwm_2_compare_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(pwm_3_gen_handle, pwm_3_compare_event));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(pwm_4_gen_handle, pwm_4_compare_event));

    // Start Timer
    ESP_ERROR_CHECK(mcpwm_timer_enable(pwm_12_timer_handle));
    ESP_ERROR_CHECK(mcpwm_timer_enable(pwm_34_timer_handle));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(pwm_12_timer_handle, MCPWM_TIMER_START_NO_STOP));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(pwm_34_timer_handle, MCPWM_TIMER_START_NO_STOP));
    // ESP_LOGI("MOTOR", "Timer started. Ticks = %d", PWM_TICKS);
}

void duty_cycle(int duty_1, int duty_2, int duty_3, int duty_4) {

    // MOTOR 1
    if (duty_1 > 100) {
        duty_1 = 0;
    }
    if (duty_1 == 0) {
        gpio_set_level(pins.ain1_1, 0);
        gpio_set_level(pins.ain2_1, 0);
    }
    else if (duty_1 < 0) {
        duty_1 = -1*duty_1;
        gpio_set_level(pins.ain1_1, 0);
        gpio_set_level(pins.ain2_1, 1);
    }
    else if (duty_1 > 0) {
        gpio_set_level(pins.ain1_1, 1);
        gpio_set_level(pins.ain2_1, 0);
    }

    // MOTOR 2
    if (duty_2 > 100) {
        duty_2 = 0;
    }
    if (duty_2 == 0) {
        gpio_set_level(pins.bin1_1, 0);
        gpio_set_level(pins.bin2_1, 0);
    }
    else if (duty_2 < 0) {
        duty_2 = -1*duty_2;
        gpio_set_level(pins.bin1_1, 0);
        gpio_set_level(pins.bin2_1, 1);
    }
    else if (duty_2 > 0) {
        gpio_set_level(pins.bin1_1, 1);
        gpio_set_level(pins.bin2_1, 0);
    }

    // MOTOR 3
    if (duty_3 > 100) {
        duty_3 = 0;
    }
    if (duty_3 == 0) {
        gpio_set_level(pins.ain1_2, 0);
        gpio_set_level(pins.ain2_2, 0);
    }
    else if (duty_3 < 0) {
        duty_3 = -1*duty_3;
        gpio_set_level(pins.ain1_2, 0);
        gpio_set_level(pins.ain2_2, 1);
    }
    else if (duty_3 > 0) {
        gpio_set_level(pins.ain1_2, 1);
        gpio_set_level(pins.ain2_2, 0);
    }

    // MOTOR 4
    if (duty_4 > 100) {
        duty_4 = 0;
    }
    if (duty_4 == 0) {
        gpio_set_level(pins.bin1_2, 0);
        gpio_set_level(pins.bin2_2, 0);
    }
    else if (duty_4 < 0) {
        duty_4 = -1*duty_4;
        gpio_set_level(pins.bin1_2, 0);
        gpio_set_level(pins.bin2_2, 1);
    }
    else if (duty_4 > 0) {
        gpio_set_level(pins.bin1_2, 1);
        gpio_set_level(pins.bin2_2, 0);
    }

    gpio_set_level(pins.stby, 1);
    uint32_t ticks_1 = duty_1 * PWM_TICKS / 100;
    uint32_t ticks_2 = duty_2 * PWM_TICKS / 100;
    uint32_t ticks_3 = duty_3 * PWM_TICKS / 100;
    uint32_t ticks_4 = duty_4 * PWM_TICKS / 100;
    // printf("Tick A: %ld\t Tick B: %ld\n", ticks_a, ticks_b);
    mcpwm_comparator_set_compare_value(pwm_1_cmpr_handle, ticks_1);
    mcpwm_comparator_set_compare_value(pwm_2_cmpr_handle, ticks_2);
    mcpwm_comparator_set_compare_value(pwm_3_cmpr_handle, ticks_3);
    mcpwm_comparator_set_compare_value(pwm_4_cmpr_handle, ticks_4);

    // vTaskDelay(pdMS_TO_TICKS(50));
}