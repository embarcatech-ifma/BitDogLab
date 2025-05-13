#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LED 12

int64_t meu_callback(alarm_id_t id, void *user_data) {
    static bool ligado = false;

    if (ligado) {
        gpio_put(LED, 0); // Apaga LED
        printf("🔴 LED desligado.\n");
    } else {
        gpio_put(LED, 1); // Acende LED
        printf("🟢 LED aceso! Alarme disparado.\n");
    }

    ligado = !ligado;

    return 3000000; // Reagenda para 3 segundos depois
}

int main() {
    stdio_init_all();
    sleep_ms(5000);
    printf("Aguardando alarme...\n");

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_put(LED, 0); // Apagado

    // Agenda o alarme para daqui 3000 ms (3 segundos)
    add_alarm_in_ms(3000, meu_callback, NULL, true);

    while (true) {
        tight_loop_contents(); // Não faz nada, só espera interrupção
    }

    return 0;
}
