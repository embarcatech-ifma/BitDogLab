#ifndef MQTT_LWIP_H
#define MQTT_LWIP_H

#include "lwip/apps/mqtt.h"

// Inicializa e conecta o cliente MQTT ao broker definido em configura_geral.h
void iniciar_mqtt_cliente(void);

// Publica uma mensagem no tópico definido (TOPICO) em configura_geral.h
void publicar_mensagem_mqtt(const char *mensagem);

// Loop de manutenção MQTT (reservado para uso futuro)
void mqtt_loop(void);

#endif
