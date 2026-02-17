#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern volatile int count_1;
extern volatile int count_2;
extern volatile int count_3;
extern volatile int count_4;


extern double ds1;
extern double ds2;
extern double ds3;
extern double ds4;

extern double v1;
extern double v2;
extern double v3;
extern double v4;

extern QueueHandle_t queue_1;
extern QueueHandle_t queue_2;
extern QueueHandle_t queue_3;
extern QueueHandle_t queue_4;

void enc_table(void);
void setup_queues(void);
void setup_isr(void);
void create_enc_timer(void);
void get_count(void);

#endif