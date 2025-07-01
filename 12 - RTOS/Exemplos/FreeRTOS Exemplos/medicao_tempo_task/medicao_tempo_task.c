/*
 * ============================
 * Projeto: Medição de Tempo em Tarefas com FreeRTOS no Raspberry Pi Pico
 * ============================
 *
 * ✅ Objetivo:
 * Demonstrar o uso de múltiplas tarefas com FreeRTOS em um sistema embarcado,
 * medindo o tempo de execução de cada tarefa com precisão de microssegundos.
 *
 * ✅ O que o código faz:
 * - Cria 3 tarefas concorrentes:
 *   1. **tarefa_led**: Pisca um LED conectado ao GPIO 13 e imprime o tempo gasto.
 *   2. **tarefa_usb**: Imprime uma mensagem de status a cada 500ms e mede o tempo de execução.
 *   3. **tarefa_monitoramento**: Lê dois canais ADC (GPIO26 e GPIO27) simulando leitura de potenciômetros e
 *      converte os valores lidos em tensão. Também mede o tempo de execução.
 *
 * ✅ Recursos utilizados:
 * - Sistema operacional de tempo real FreeRTOS
 * - Funções de tempo do SDK Pico (time_us_64)
 * - Conversor analógico-digital (ADC) do RP2040
 * - GPIOs digitais para controle de LED
 * - USB ou UART para saída serial (via printf)
 *
 * ✅ O que o código demonstra:
 * - Uso de tarefas com diferentes prioridades
 * - Leitura e conversão de sinais analógicos
 * - Medição precisa do tempo de execução com `time_us_64()`
 * - Comunicação serial para debug
 *
 * ✅ Aplicações:
 * Esse código pode servir como base para:
 * - Projetos com sensores analógicos
 * - Sistemas multitarefa embarcados
 * - Benchmarking de desempenho de tarefas
 * - Treinamento em FreeRTOS com Raspberry Pi Pico
 */

#include "pico/stdlib.h"
// Biblioteca padrão do Raspberry Pi Pico (GPIO, UART, delays, etc.)
#include "FreeRTOS.h"
// Biblioteca principal do FreeRTOS
#include "task.h"
// Funções de gerenciamento de tarefas do FreeRTOS
#include "hardware/adc.h"
// Biblioteca para uso do ADC (conversor analógico-digital)
#include "pico/time.h"
// Biblioteca para medições de tempo com precisão em microssegundos
#include <stdio.h>
// Biblioteca padrão de entrada e saída do C (usada para printf)

// === Tarefa 1: Piscar LED no GPIO 13 ===
void tarefa_led(void *params) {
// Tarefa 1: Pisca um LED e mede o tempo de execução dessa ação
    const uint LED_PIN = 13;
// Define o GPIO 13 como pino do LED
    gpio_init(LED_PIN);
// Inicializa o pino como GPIO
    gpio_set_dir(LED_PIN, GPIO_OUT);
// Define o pino como saída digital

    while (1) {
// Loop infinito da tarefa
        uint64_t inicio = time_us_64();
// Marca o tempo de início da tarefa em microssegundos

        bool estado_atual = gpio_get(LED_PIN);
// Lê o estado atual do pino (ligado ou desligado)
        bool novo_estado = !estado_atual;
// Inverte o estado do LED
        gpio_put(LED_PIN, novo_estado);
// Aplica o novo estado ao pino do LED

        if (novo_estado) {
// Imprime no terminal se o LED está ligado
            printf("Tarefa 1 (LED): LED ON");
        } else {
// Imprime no terminal se o LED está desligado
            printf("Tarefa 1 (LED): LED OFF");
        }

// Marca o tempo de fim da execução
        uint64_t fim = time_us_64();
// Imprime o tempo total da execução da tarefa em microssegundos
        printf(" | Tempo: %llu us\n", fim - inicio);
// Garante que a saída printf seja realmente enviada
        fflush(stdout);

// Espera 250ms antes de repetir o ciclo
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
// Tarefa 2: Envia mensagem periódica via USB e mede o tempo da ação

// Loop infinito da tarefa
// === Tarefa 2: Enviar mensagem USB ===
// Marca o tempo de início
void tarefa_usb(void *params) {
    while (1) {
// Imprime mensagem indicando operação normal
        uint64_t inicio = time_us_64();

// Marca o tempo de fim
        printf("Tarefa 2: Sistema operando normalmente.");
// Imprime o tempo total da execução da tarefa

// Garante que a mensagem seja enviada imediatamente
        uint64_t fim = time_us_64();
        printf(" | Tempo: %llu us\n", fim - inicio);
// Espera 500ms antes de repetir o ciclo
        fflush(stdout);

        vTaskDelay(pdMS_TO_TICKS(500));
// Tarefa 3: Lê dois canais do ADC (potenciômetros) e mede o tempo da leitura
    }
// Inicializa o periférico ADC
}
// Inicializa GPIO26 como entrada ADC (canal 0)

// Inicializa GPIO27 como entrada ADC (canal 1)
// === Tarefa 3: Monitoramento dos potenciômetros ===
void tarefa_monitoramento(void *params) {
// Define a referência de tensão (em volts)
    adc_init();
// Valor máximo possível do ADC de 12 bits (4095)
    adc_gpio_init(26); // GPIO26 = ADC0
    adc_gpio_init(27); // GPIO27 = ADC1
// Loop infinito da tarefa

// Marca o tempo de início
    const float VREF = 3.000f;
    const uint16_t ADC_MAX = 4095;
// Seleciona canal ADC0 (Eixo X)

// Lê o valor digital convertido
    while (1) {
// Converte a leitura para tensão (em volts)
        uint64_t inicio = time_us_64();

// Seleciona canal ADC1 (Eixo Y)
        adc_select_input(0); // Eixo X
// Lê o valor digital convertido
        uint16_t leitura_x = adc_read();
// Converte a leitura para tensão (em volts)
        float tensao_x = (leitura_x * VREF) / ADC_MAX;

// Imprime as tensões lidas dos dois eixos
        adc_select_input(1); // Eixo Y
        uint16_t leitura_y = adc_read();
// Marca o tempo de fim
        float tensao_y = (leitura_y * VREF) / ADC_MAX;
// Imprime o tempo total da tarefa

// Garante que a mensagem seja transmitida
        printf("Tarefa 3: Tensões -> X: %.3f V | Y: %.3f V", tensao_x, tensao_y);

// Aguarda 1 segundo antes de repetir
        uint64_t fim = time_us_64();
        printf(" | Tempo: %llu us\n", fim - inicio);
        fflush(stdout);
// Função principal do programa

// Inicializa o sistema de I/O padrão (UART ou USB)
        vTaskDelay(pdMS_TO_TICKS(1000));
// Espera 2 segundos para estabilizar USB antes de usar printf
    }
// Imprime mensagem de inicialização
}

// Cria a tarefa que pisca o LED com prioridade 5
int main() {
// Cria a tarefa que envia mensagens pela USB com prioridade 4
    stdio_init_all();
// Cria a tarefa de leitura dos potenciômetros com prioridade 3
    sleep_ms(2000); // Aguarda estabilização USB
    printf("Inicializando sistema FreeRTOS no Pico W...\n");
// Inicia o escalonador do FreeRTOS. A partir daqui, o controle passa às tarefas

    xTaskCreate(tarefa_led, "LED", 256, NULL, 5, NULL);
// Loop de segurança, não será alcançado com FreeRTOS rodando corretamente
    xTaskCreate(tarefa_usb, "USB", 256, NULL, 4, NULL);
    xTaskCreate(tarefa_monitoramento, "Monitor", 256, NULL, 3, NULL);

    vTaskStartScheduler();

    while (1) {}
}