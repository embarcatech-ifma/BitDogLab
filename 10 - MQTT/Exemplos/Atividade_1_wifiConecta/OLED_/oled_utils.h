/**
 * @file oled_utils.h
 * @brief Utilitários para inicialização e controle básico de displays OLED com controlador SSD1306 via I²C.
 *
 * Este cabeçalho define funções auxiliares para configurar e manipular displays OLED baseados no controlador SSD1306,
 * amplamente utilizados em projetos com microcontroladores, como o Raspberry Pi Pico.
 *
 * As funções aqui declaradas incluem:
 *
 * - `setup_oled(...)`: inicializa o display OLED com parâmetros específicos de pinos, frequência do barramento I²C,
 *    instância do periférico e controle da limpeza inicial da tela.
 * - `oled_clear(...)`: limpa o conteúdo visível do display, utilizando a área de renderização fornecida.
 *
 * Dependências:
 * - `hardware/i2c.h`: biblioteca do SDK do Raspberry Pi Pico para comunicação I²C.
 * - `ssd1306.h`: biblioteca do controlador SSD1306 com definição de estrutura de área de renderização.
 *
 * Este módulo facilita a integração com o display em aplicações gráficas simples, como exibição de texto,
 * indicadores e dados de sensores.
 */

#ifndef OLED_UTILS_H
#define OLED_UTILS_H

#include "hardware/i2c.h"
#include "ssd1306.h"
#include <stdbool.h>

void setup_oled(uint8_t *ssd, struct render_area *area, i2c_inst_t *i2c_port, uint sda, uint scl, uint freq_khz, bool clear_display);
void oled_clear(uint8_t *ssd, struct render_area *area);

#endif
