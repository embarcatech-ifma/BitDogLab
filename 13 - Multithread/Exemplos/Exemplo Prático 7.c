/*
================================================================================
Exemplo prático 7: Projeto Integrador Final
(Item 7 do E-Book)
=======================================================
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// --- Definições de pinos ---
#define VRX_PIN 27 // ADC1
#define VRY_PIN 26 // ADC0
#define BUTTON_PIN 5
#define LED_PIN 13

// --- Handles globais ---
QueueHandle_t joystick_queue;
SemaphoreHandle_t usb_mutex;
SemaphoreHandle_t button_semaphore;

// --- Tarefa: Leitura do Joystick e envio por fila ---
void sensor_task(void *param) {
  adc_init();
  adc_gpio_init(VRX_PIN);
  adc_gpio_init(VRY_PIN);
  while (1) {
    uint16_t vrx = 0, vry = 0;
    adc_select_input(1); // VRX
    vrx = adc_read();
    adc_select_input(0); // VRY
    vry = adc_read();
    uint16_t data[2] = {vrx, vry};
    xQueueSend(joystick_queue, &data, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// --- Tarefa: Monitoramento do botão e liberação de semáforo ---
void button_task(void *param) {
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN); // Evita flutuação
  while (1) {
    if (!gpio_get(BUTTON_PIN)) { // Pressionado (nível baixo)
      xSemaphoreGive(button_semaphore); // Sinaliza evento
      vTaskDelay(pdMS_TO_TICKS(200)); // Debounce simples
    } else {
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

// --- Tarefa: Atua sobre LED e escreve na serial protegida por mutex ---
void actuator_task(void *param) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  uint16_t data[2];
  while (1) {
    
    // Se evento do botão for detectado
    if (xSemaphoreTake(button_semaphore, 0) == pdTRUE) {
      for (int i = 0; i < 3; i++) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
    
    // Se dados do joystick forem recebidos
    if (xQueueReceive(joystick_queue, &data, pdMS_TO_TICKS(10)) == pdTRUE) {
      if (xSemaphoreTake(usb_mutex, pdMS_TO_TICKS(100))) {
        printf("Joystick - VRX: %d, VRY: %d\n", data[0], data[1]);
        xSemaphoreGive(usb_mutex);
      }
      
      // Indicação visual simples: acende LED se valores altos
      if (data[0] > 3000 || data[1] > 3000) {
        gpio_put(LED_PIN, 1);
      } else {
        gpio_put(LED_PIN, 0);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// --- Função principal ---
int main() {
  stdio_init_all();
  
  // Criação dos recursos
  joystick_queue = xQueueCreate(5, sizeof(uint16_t[2]));
  usb_mutex = xSemaphoreCreateMutex();
  button_semaphore = xSemaphoreCreateBinary();
  
  // Criação das tarefas
  xTaskCreate(sensor_task, "Sensor", 256, NULL, 1, NULL);
  xTaskCreate(button_task, "Button", 256, NULL, 1, NULL);
  xTaskCreate(actuator_task, "Actuator", 512, NULL, 1, NULL);
  
  // Inicia o escalonador
  vTaskStartScheduler();
  
  while (true);
  return 0;
}
