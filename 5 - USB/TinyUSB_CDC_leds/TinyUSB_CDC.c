#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/gpio.h"

// Define os pinos dos LEDs
#define LED_PIN_13 13
#define LED_PIN_11 11
#define LED_PIN_12 12

// Função para inicializar os LEDs
void init_leds() {
    // Configura o pino 13
    gpio_init(LED_PIN_13);
    gpio_set_dir(LED_PIN_13, GPIO_OUT);
    gpio_pull_up(LED_PIN_13);
    gpio_put(LED_PIN_13, 0); // LED apagado (nível alto em pull-up)

    // Configura o pino 11
    gpio_init(LED_PIN_11);
    gpio_set_dir(LED_PIN_11, GPIO_OUT);
    gpio_pull_up(LED_PIN_11);
    gpio_put(LED_PIN_11, 0); // LED apagado (nível alto em pull-up)

    // Configura o pino 12
    gpio_init(LED_PIN_12);
    gpio_set_dir(LED_PIN_12, GPIO_OUT);
    gpio_pull_up(LED_PIN_12);
    gpio_put(LED_PIN_12, 0); // LED apagado (nível alto em pull-up)
}

// Função para acender um LED por 3 segundos
void turn_on_led(int pin) {
    gpio_put(pin, 1); // Acende o LED (nível baixo)
    sleep_ms(3000);   // Aguarda 3 segundos
    gpio_put(pin, 0); // Apaga o LED (nível alto)
}

int main() {
    // Inicializa o USB
    stdio_init_all();

    // Aguarda a conexão USB com o host
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }
    printf("USB conectado!\n");

    // Inicializa os LEDs
    init_leds();

    // Loop principal
    while (true) {
        if (tud_cdc_available()) { // Verifica se há dados disponíveis
            char buf[64]; // Buffer para armazenar os dados recebidos
            uint32_t count = tud_cdc_read(buf, sizeof(buf)); // Lê os dados
            buf[count] = '\0'; // Adiciona terminador de string

            // Verifica os valores recebidos e acende o LED correspondente
            if (strcmp(buf, "13") == 0) {
                turn_on_led(LED_PIN_13);
            } else if (strcmp(buf, "11") == 0) {
                turn_on_led(LED_PIN_11);
            } else if (strcmp(buf, "12") == 0) {
                turn_on_led(LED_PIN_12);
            }

            // Ecoa os dados recebidos de volta ao host
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
        tud_task(); // Executa tarefas USB
    }

    return 0;
}