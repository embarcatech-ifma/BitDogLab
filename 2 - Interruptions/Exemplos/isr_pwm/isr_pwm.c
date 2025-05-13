#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#define PWM_GPIO 21               // Buzzer da BitDogLab
#define PWM_FREQ_HZ 1000          // Frequência do bipe em Hz (1 kHz)
#define CLOCK_DIV 2.0f            // Divisor ajustado para evitar overflow
#define PWM_WRAP (uint16_t)(125000000 / (PWM_FREQ_HZ * CLOCK_DIV))  // = 62500 (válido para 16 bits)

#define CICLOS_PARA_1S PWM_FREQ_HZ  // 1000 ciclos de PWM = 1 segundo a 1kHz

volatile int contador = 0;
volatile bool tocando = false;

// Interrupção de fim de ciclo PWM
void pwm_wrap_handler() {
    uint slice = pwm_gpio_to_slice_num(PWM_GPIO);
    pwm_clear_irq(slice);

    contador++;

    if (contador == 0) return; // segurança

    // A cada 1000 ciclos (~1s a 1kHz), inicia bipe
    if (contador >= CICLOS_PARA_1S) {
        contador = 0;
        tocando = true;
        pwm_set_gpio_level(PWM_GPIO, PWM_WRAP / 2);  // Duty 50%
        printf("🔊 BEEP!\n");
    }

    // Após 100 ciclos (~100 ms), encerra bipe
    if (tocando && contador == 100) {
        pwm_set_gpio_level(PWM_GPIO, 0);  // Silencia buzzer
        tocando = false;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("🌀 PWM + IRQ no GPIO21 (Buzzer BitDogLab)\n");

    // Configura pino como saída PWM
    gpio_set_function(PWM_GPIO, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PWM_GPIO);

    // Configura PWM
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, CLOCK_DIV);
    pwm_config_set_wrap(&cfg, PWM_WRAP);
    pwm_init(slice, &cfg, true);

    // Começa com buzzer desligado
    pwm_set_gpio_level(PWM_GPIO, 0);

    // Configura interrupção do PWM
    pwm_clear_irq(slice);
    pwm_set_irq_enabled(slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_wrap_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    while (true) {
        tight_loop_contents();  // Espera interrupção
    }

    return 0;
}
