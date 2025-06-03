/**
 * @file oled_utils.c
 * @brief Funções utilitárias para configuração e manipulação básica de displays OLED com controlador SSD1306 via I²C.
 *
 * Este módulo fornece duas funções essenciais para iniciar a comunicação com o display OLED 
 * e apagar seu conteúdo, sendo projetado para sistemas embarcados como o Raspberry Pi Pico.
 *
 * Funcionalidades implementadas:
 * - Inicialização do barramento I²C e do display OLED.
 * - Definição da área de renderização do display.
 * - Limpeza total do conteúdo gráfico exibido.
 *
 * Uso comum em sistemas que utilizam tela para exibir mensagens de status, dados sensoriais ou feedbacks gráficos.
 *
 * Dependências:
 * - `hardware/i2c.h` e `gpio.h` do Pico SDK
 * - `ssd1306.h` para funções e definições específicas do controlador do display
 * - `oled_utils.h` para a interface dessas funções
 */

#include "oled_utils.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include <string.h>  // Para uso da função memset()
#include "ssd1306_i2c.h"

/**
 * @brief Inicializa o display OLED via I²C com os parâmetros fornecidos e define a área de renderização.
 *
 * @param ssd            Ponteiro para o buffer de dados do display.
 * @param area           Ponteiro para a estrutura da área de renderização.
 * @param i2c_port       Instância I²C a ser usada (ex: i2c0 ou i2c1).
 * @param sda            Pino GPIO para SDA.
 * @param scl            Pino GPIO para SCL.
 * @param freq_khz       Frequência de operação do barramento I²C em KHz.
 * @param clear_display  Se verdadeiro, limpa o display após inicialização.
 */
void setup_oled(uint8_t *ssd, struct render_area *area, i2c_inst_t *i2c_port, uint sda, uint scl, uint freq_khz, bool clear_display)
{
    // Inicializa o barramento I²C com a frequência especificada (convertida para Hz)
    i2c_init(i2c_port, freq_khz * 1000);

    // Configura os pinos como função I²C
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);

    // Habilita resistores de pull-up nos pinos SDA e SCL
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Inicializa o display OLED (sequência de comandos padrão SSD1306)
    ssd1306_init();

    // Define os limites da área de renderização (tela inteira)
    area->start_column = 0;
    area->end_column = ssd1306_width - 1;
    area->start_page = 0;
    area->end_page = ssd1306_n_pages - 1;

    // Calcula o tamanho do buffer necessário para essa área
    calculate_render_area_buffer_length(area);

    // Se solicitado, limpa completamente a tela após a inicialização
    if (clear_display) {
        oled_clear(ssd, area);
    }
}

/**
 * @brief Limpa o conteúdo gráfico do display (zera o buffer e atualiza a tela).
 *
 * @param ssd   Ponteiro para o buffer gráfico.
 * @param area  Estrutura com as coordenadas da área a ser renderizada (normalmente toda a tela).
 *
 * Esta função preenche o buffer com zeros (apagando visualmente a tela) e o envia ao display.
 */
void oled_clear(uint8_t *ssd, struct render_area *area)
{
    // Preenche o buffer com zeros (todos os pixels apagados)
    memset(ssd, 0, ssd1306_buffer_length);

    // Envia o buffer atualizado para o display
    render_on_display(ssd, area);
}
