#include <stdio.h>               // Biblioteca necessária para usar printf()
#include "pico/stdlib.h"         // Biblioteca padrão para funções de delay, printf via USB, GPIO etc.
#include "hardware/watchdog.h"   // Biblioteca específica do RP2040 para controle do Watchdog Timer

int main() {
    stdio_init_all();            // Inicializa o subsistema de entrada/saída USB (CDC serial)
    sleep_ms(2000);              // Aguarda 2 segundos para garantir que a porta USB esteja estabilizada

    printf("Sistema iniciou. Ativando watchdog (5s)...\n");

    // Ativa o Watchdog Timer com tempo limite de 5000 ms (5 segundos)
    // O segundo argumento "false" significa que NÃO queremos interrupção antes do reset
    watchdog_enable(5000, false);

    while (true) {
        printf("Alimentando watchdog...\n");

        // Reinicia a contagem do watchdog. Se esta função for chamada dentro do tempo,
        // o microcontrolador não reinicia. É como "alimentar o cachorro".
        watchdog_update();

        sleep_ms(6000);  // Aguarda 1 segundo antes da próxima "alimentação"
    }

    return 0;  // Nunca alcançado, pois o loop é infinito
}
