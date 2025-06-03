#ifndef CONFIGURA_GERAL_H
#define CONFIGURA_GERAL_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include <stdint.h>
#include <stdbool.h>
#include "pico/mutex.h"

//LED RGB
#define LED_R 12
#define LED_G 11
#define LED_B 13
#define PWM_STEP 0xFFFF
//#define PWM_STEP (1 << 8)

//Pinos I2C
#define SDA_PIN 14
#define SCL_PIN 15

#define TEMPO_CONEXAO 2000
#define TEMPO_MENSAGEM 2000
#define TAM_FILA 16

#define WIFI_SSID "SUA REDE"
#define WIFI_PASS "SUA SENHA"
#define MQTT_BROKER_IP "ENDEREÇO IP DO MOSQUITTO"
#define MQTT_BROKER_PORT 1883
#define TOPICO "pico/PING"
#define INTERVALO_PING_MS 5000


// Buffers globais para OLED
extern uint8_t buffer_oled[];
extern struct render_area area;

void setup_init_oled(void);
void exibir_e_esperar(const char *mensagem, int linha_y);

#endif