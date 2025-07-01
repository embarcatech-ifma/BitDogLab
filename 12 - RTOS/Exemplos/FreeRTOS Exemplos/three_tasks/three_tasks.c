#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/adc.h"
#include <stdio.h>

// === Tarefa 1: Piscar LED no GPIO 13 ===
void tarefa_led(void *params) {
    const uint LED_PIN = 13;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        bool estado_atual = gpio_get(LED_PIN);
        bool novo_estado = !estado_atual;
        gpio_put(LED_PIN, novo_estado);

        printf("Tarefa 1 (LED): LED %s\n", novo_estado ? "ON" : "OFF");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// === Tarefa 2: Enviar mensagem USB ===
void tarefa_usb(void *params) {
    while (1) {
        printf("Tarefa 2: Sistema operando normalmente.\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// === Tarefa 3: Monitoramento dos potenciômetros (ADC) ===
void tarefa_monitoramento(void *params) {
    const float VREF = 3.000f;
    const uint16_t ADC_MAX = 4095;

    adc_init();
    adc_gpio_init(26); // GPIO26 = ADC0
    adc_gpio_init(27); // GPIO27 = ADC1

    while (1) {
        adc_select_input(0);
        uint16_t leitura_x = adc_read();
        float tensao_x = (leitura_x * VREF) / ADC_MAX;

        adc_select_input(1);
        uint16_t leitura_y = adc_read();
        float tensao_y = (leitura_y * VREF) / ADC_MAX;

        printf("Tarefa 3: Tensões -> X: %.3f V | Y: %.3f V\n", tensao_x, tensao_y);
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// === Função principal ===
int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Inicializando sistema FreeRTOS no Pico W...\n");

    xTaskCreate(tarefa_led, "LED", 256, NULL, 5, NULL);
    xTaskCreate(tarefa_usb, "USB", 256, NULL, 4, NULL);
    xTaskCreate(tarefa_monitoramento, "Monitor", 256, NULL, 3, NULL);

    vTaskStartScheduler();

    while (1) {}
}
