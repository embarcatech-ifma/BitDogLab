#ifndef ESTADO_MQTT_H
#define ESTADO_MQTT_H

#include <stdint.h>
#include <stdbool.h>

// Variáveis compartilhadas entre arquivos
extern uint32_t ultimo_ip_bin;
extern bool mqtt_iniciado;

// Buffer OLED e área global
extern uint8_t buffer_oled[];
extern struct render_area area;

//Variável de controle do ping
extern volatile uint32_t intervalo_ping_ms;

void set_novo_intervalo_ping(uint32_t novo_intervalo);

#endif
