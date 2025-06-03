#ifndef RGB_PWM_CONTROL_H
#define RGB_PWM_CONTROL_H

#include "configura_geral.h"

void init_rgb_pwm();
void set_rgb_pwm(uint16_t r_val, uint16_t g_val, uint16_t b_val);

#endif