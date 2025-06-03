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

#endif
