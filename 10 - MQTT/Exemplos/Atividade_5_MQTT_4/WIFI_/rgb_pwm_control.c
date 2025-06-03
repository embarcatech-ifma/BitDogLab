#include "rgb_pwm_control.h"

static uint slice_r, slice_g, slice_b;

void init_rgb_pwm() {
    
    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_G, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);

    slice_r = pwm_gpio_to_slice_num(LED_R);
    slice_g = pwm_gpio_to_slice_num(LED_G);
    slice_b = pwm_gpio_to_slice_num(LED_B);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);

    pwm_init(slice_r, &config, true);
    pwm_init(slice_g, &config, true);
    pwm_init(slice_b, &config, true);
}

void set_rgb_pwm(uint16_t r_val, uint16_t g_val, uint16_t b_val) {
    pwm_set_gpio_level(LED_R, r_val);
    pwm_set_gpio_level(LED_G, g_val);
    pwm_set_gpio_level(LED_B, b_val);
}