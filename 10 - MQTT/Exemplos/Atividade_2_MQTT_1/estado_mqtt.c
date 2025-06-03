/**
 * @file estado_mqtt.c
 * @brief Estado compartilhado do cliente MQTT e estruturas globais do OLED.
 *
 * Este arquivo armazena **variáveis globais** utilizadas em múltiplos arquivos do sistema,
 * facilitando o compartilhamento de estado entre o código principal (`main.c`) e os
 * módulos auxiliares, como `main_auxiliar.c` e `mqtt_lwip.c`.
 *
 * Ele define:
 * - O último endereço IP recebido (`ultimo_ip_bin`), utilizado para iniciar o cliente MQTT;
 * - Um flag (`mqtt_iniciado`) que garante que o cliente MQTT só será iniciado uma vez;
 * - Um buffer de vídeo (`buffer_oled`) para escrita no display OLED;
 * - A estrutura `area`, que define a região da tela sendo desenhada.
 *
 * O objetivo é **centralizar informações compartilhadas** entre os diversos arquivos
 * do projeto, evitando duplicação e facilitando a manutenção e legibilidade do código.
 */

#include "estado_mqtt.h"        // Declaração das variáveis externas
#include "ssd1306_i2c.h"        // Define tamanho do buffer e estrutura de renderização

// ================================
// DEFINIÇÕES GLOBAIS ÚNICAS
// ================================

/**
 * @brief Endereço IP mais recente recebido via núcleo 1.
 *
 * Usado pelo núcleo 0 para verificar se a rede já obteve um IP válido
 * e então iniciar o cliente MQTT.
 */
uint32_t ultimo_ip_bin = 0;

/**
 * @brief Flag de controle que indica se o cliente MQTT já foi iniciado.
 *
 * Isso evita múltiplas tentativas de iniciar o cliente MQTT após o recebimento do IP.
 * Deve ser verificado no loop principal.
 */
bool mqtt_iniciado = false;

/**
 * @brief Buffer de vídeo para o display OLED.
 *
 * Este buffer contém os dados de pixels que serão renderizados na tela.
 * Seu tamanho é definido pela função `ssd1306_buffer_length`, de acordo com
 * a resolução do display (tipicamente 128x64).
 */
uint8_t buffer_oled[ssd1306_buffer_length];

/**
 * @brief Estrutura que define a área da tela a ser desenhada.
 *
 * Usada pelas funções de renderização para aplicar atualizações no OLED,
 * como `render_on_display()`.
 */
struct render_area area;
