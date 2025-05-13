#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#define LED_PIN 12

// Tarefa 1: acessa o recurso, mas não libera
void tarefa1() {
    static bool recurso_ocupado = false;

    if (!recurso_ocupado) {
        recurso_ocupado = true;
        printf("🔴 Tarefa 1 está usando o recurso (e não libera)...\n");
        gpio_put(LED_PIN, 1);  // Acende LED
        // Simula travamento: entra em loop infinito
        while (true) {
            sleep_ms(100);
        }
    }
}

// Tarefa 2: tenta acessar o recurso
void tarefa2() {
    printf("🟡 Tarefa 2 tentando usar o recurso...\n");
    gpio_put(LED_PIN, 0);  // Apaga LED (se conseguir rodar)
}

int main() {
    stdio_init_all();

    // Aguarda conexão da USB Serial (ou timeout de 10 segundos)
    int tempo_espera = 0;
    while (!stdio_usb_connected()) {
        sleep_ms(100);
        tempo_espera += 100;
        if (tempo_espera >= 10000) break;
    }
    sleep_ms(500);

    // Inicializa LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    printf("🔵 Sistema iniciado. Watchdog ativado (tempo = 6s)...\n");

    // Ativa watchdog: 6 segundos
    watchdog_enable(6000, false);  // Reinicia o sistema se não for alimentado

    while (true) {
        tarefa1();
        sleep_ms(500);
        watchdog_update();  // "Alimenta" o watchdog

        tarefa2();
        sleep_ms(500);
        watchdog_update();
    }

    return 0;
}
