/*
================================================================================
Exemplo prático 2: Projeto com múltiplos produtores e múltiplos consumidores e uma fila
(Item 4.8 do E-Book)
=======================================================
*/

#include "stdio.h" // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h" // Biblioteca da Raspberry Pi Pico para funções básicas
#include "hardware/adc.h" // Biblioteca para controle dos conversores analógico - digitais (ADC)
#include "hardware/gpio.h" // Biblioteca para controle dos pinos GPIO
#include "hardware/timer.h" // Biblioteca para controle de temporizadores
#include "FreeRTOS.h" // Cabeçalho principal do FreeRTOS
#include "task.h" // Gerenciamento de tarefas no FreeRTOS
#include "queue.h" // Suporte a filas do FreeRTOS

// --- Definições de pinos ---
#define ADC_Y 0 // GPIO26 (VRy)
#define ADC_X 1 // GPIO27 (VRx)
#define LED_X 12 // LED indicador eixo X
#define LED_Y 13 // LED indicador eixo Y
#define TRIG_PIN 16 // Pino de disparo do sensor ultrassônico
#define ECHO_PIN 17 // Pino de recepção do sensor ultrassônico

// --- Enum para origem dos dados ---
typedef enum {
  SENSOR_JOYSTICK = 1, // Identificador para dados do joystick
  SENSOR_ULTRASSONICO = 2 // Identificador para dados do sensor ultrassônico
} SensorOrigem_t;

// --- Estrutura de dados da fila ---
typedef struct {
  SensorOrigem_t origem;
  union {
    struct {
      uint16_t x;
      uint16_t y;
    } joystick;
    float distancia_cm;
  } dados;
} SensorData_t;
QueueHandle_t fila_sensores; // Fila compartilhada entre todas as tarefas

// --- Tarefa: Leitura do Joystick ---
void tarefa_joystick(void *param) {
  SensorData_t leitura;
  adc_init();
  adc_gpio_init(26); // VRy
  adc_gpio_init(27); // VRx
  while (true) {
    leitura.origem = SENSOR_JOYSTICK;
    adc_select_input(ADC_X);
    leitura.dados.joystick.x = adc_read();
    adc_select_input(ADC_Y);
    leitura.dados.joystick.y = adc_read();
    xQueueSend(fila_sensores, &leitura, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// --- Função auxiliar: medir tempo em microsegundos para sinal de eco ---
uint32_t medir_pulso() {
  absolute_time_t start = get_absolute_time();
  while (gpio_get(ECHO_PIN) == 0) {
    if (absolute_time_diff_us(start, get_absolute_time()) > 30000) return 0;
  }
  absolute_time_t high_start = get_absolute_time();
  while (gpio_get(ECHO_PIN) == 1) {
    if (absolute_time_diff_us(high_start, get_absolute_time()) > 30000) return 0;
  }
  absolute_time_t end = get_absolute_time();
  return absolute_time_diff_us(high_start, end);
}

// --- Tarefa: Sensor Ultrassônico ---
void tarefa_ultrassonico(void *param) {
  SensorData_t leitura;
  gpio_init(TRIG_PIN);
  gpio_set_dir(TRIG_PIN, GPIO_OUT);
  gpio_put(TRIG_PIN, 0);
  gpio_init(ECHO_PIN);
  gpio_set_dir(ECHO_PIN, GPIO_IN);
  while (true) {
    leitura.origem = SENSOR_ULTRASSONICO;
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);
    uint32_t duracao = medir_pulso();
    leitura.dados.distancia_cm = duracao * 0.0343f / 2.0f;
    xQueueSend(fila_sensores, &leitura, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

// --- Tarefa: Consome dados do joystick e acende LEDs ---
void tarefa_controle_led(void *param) {
  SensorData_t dado;
  while (true) {
    if (xQueueReceive(fila_sensores, &dado, portMAX_DELAY) == pdPASS) {
      if (dado.origem == SENSOR_JOYSTICK) {
        gpio_put(LED_X, dado.dados.joystick.x > 3000);
        gpio_put(LED_Y, dado.dados.joystick.y > 3000);
      }
    }
  }
}

// --- Tarefa: Consome dados do ultrassônico e imprime na serial ---
void tarefa_distancia(void *param) {
  SensorData_t dado;
  while (true) {
    if (xQueueReceive(fila_sensores, &dado, portMAX_DELAY) == pdPASS) {
      if (dado.origem == SENSOR_ULTRASSONICO) {
        printf("Distância: %.2f cm\n", dado.dados.distancia_cm);
      }
    }
  }
}

// --- Função principal ---
int main() {
  stdio_init_all();
  gpio_init(LED_X);
  gpio_set_dir(LED_X, GPIO_OUT);
  gpio_init(LED_Y);
  gpio_set_dir(LED_Y, GPIO_OUT);
  fila_sensores = xQueueCreate(10, sizeof(SensorData_t));
  if (fila_sensores != NULL) {
    xTaskCreate(tarefa_joystick, "Joystick", 1024, NULL, 2, NULL);
    xTaskCreate(tarefa_ultrassonico, "Ultrassonico", 1024, NULL, 2, NULL);
    xTaskCreate(tarefa_controle_led, "LEDs", 1024, NULL, 1, NULL);
    xTaskCreate(tarefa_distancia, "Distancia", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
  } else {
    while (true) {
      gpio_put(LED_X, 1);
      sleep_ms(200);
      gpio_put(LED_X, 0);
      sleep_ms(200);
    }
  }
  return 0;
}
