/**
 * @file conexao.h
 * @brief Interface do módulo Wi-Fi no núcleo 1.
 */

#ifndef CONEXAO_H
#define CONEXAO_H

#include "configura_geral.h"

void conectar_wifi(void);
void monitorar_conexao_e_reconectar(void);
bool wifi_esta_conectado(void);
void enviar_status_para_core0(uint16_t status, uint16_t tentativa);
void enviar_ip_para_core0(uint8_t *ip);

#endif
