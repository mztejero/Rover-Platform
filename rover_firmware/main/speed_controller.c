#include "motor.h"
#include "encoder.h"

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define Kp 30
#define Ki 30
#define Kd 0

#define Kc 30
#define Ke 1

double e1_sum = 0;
double e2_sum = 0;
double e3_sum = 0;
double e4_sum = 0;

double e1_prev = 0;
double e2_prev = 0;
double e3_prev = 0;
double e4_prev = 0;

double e_sum_threshold = 120 / Ki;

int speed_pid(double v, double v_ref, double *e_sum, double *e_prev) {

    double e = v_ref - v;

    if (fabs(v_ref) < 1e-2) {
        *e_sum = 0;
        return 0;
    }

    *e_sum += e * 0.1;

    if (*e_sum > e_sum_threshold) {
        *e_sum = e_sum_threshold;
    }
    else if (*e_sum < -e_sum_threshold) {
        *e_sum = -e_sum_threshold;
    }

    int8_t sign = 0;
    if (e != 0) {
        sign = e / fabs(e);
    }

    double u_pid = Kp * e + Ki * (*e_sum) + Kd * (*e_prev);
    double u = u_pid + sign * Kc * exp(- Ke * fabs(v)) * e;
    
    if (u > 100) {
        u = 100;
    }
    else if (u < -100) {
        u = -100;
    }

    *e_prev = e;

    // printf("e: %f, e_sum: %f\n", e, e_sum);

    return (int)lround(u);
}

void set_motor_speed(double v1_ref, double v2_ref, double v3_ref, double v4_ref) {
    int duty_1 = speed_pid(v1, v1_ref, &e1_sum, &e1_prev);
    int duty_2 = speed_pid(v2, v2_ref, &e2_sum, &e2_prev);
    int duty_3 = speed_pid(v3, v3_ref, &e3_sum, &e3_prev);
    int duty_4 = speed_pid(v4, v4_ref, &e4_sum, &e4_prev);
    duty_cycle(duty_1, duty_2, duty_3, duty_4);
    // printf("duty 1: %d, duty 2: %d, duty 3: %d, duty 4: %d\n", duty_1, duty_2, duty_3, duty_4);
}