#include <stdio.h>
#include "pico/stdlib.h"

#define LED_VERMELHO 13  // LED vermelho da BitDogLab (ânodo comum)

int main() {
    stdio_init_all();         // Inicializa USB CDC
    sleep_ms(2000);           // Aguarda USB estabilizar
    printf("🟢 Pico pronto. Digite algo no terminal USB...\n");

    // Configura LED como saída (inicialmente apagado: nível alto)
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0);  // Apagado (ânodo comum)

    while (true) {
        // Espera bloqueando até receber um caractere
        int ch = getchar();
        char c = (char)ch;

        // Mostra o caractere recebido e seu código ASCII
        printf("🔹 Recebido: '%c' (ASCII: %d)\n", c, ch);

        // Acende LED por 200ms
        gpio_put(LED_VERMELHO, 0);  // Liga (nível baixo)
        sleep_ms(200);
        gpio_put(LED_VERMELHO, 1);  // Desliga (nível alto)
    }

    return 0;
}
