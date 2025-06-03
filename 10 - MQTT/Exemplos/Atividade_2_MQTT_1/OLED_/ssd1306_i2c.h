/**
 * @file ssd1306_i2c.h
 * @brief Definições e utilitários para controle de displays OLED SSD1306 via barramento I²C.
 *
 * Este cabeçalho contém constantes de comando, configurações de interface e estruturas auxiliares
 * utilizadas no controle de displays OLED baseados no controlador SSD1306, operando via interface I²C.
 * 
 * Principais elementos incluídos:
 * 
 * - Definições de largura, altura e endereço I²C do display.
 * - Conjuntos de comandos SSD1306 para configuração e controle (ex: contraste, rolagem, mapeamento de segmentos).
 * - Cálculos relacionados à memória de vídeo (buffer) e altura das páginas.
 * - Estrutura `render_area` para delimitar áreas específicas da tela a serem renderizadas.
 * - Estrutura `ssd1306_t` que encapsula propriedades da tela e ponteiros para buffers.
 * - Funções para desenhar texto UTF-8: `ssd1306_draw_utf8_string()` e `ssd1306_draw_utf8_multiline()`.
 *
 * Este módulo é base para projetos gráficos embarcados com microcontroladores, como o Raspberry Pi Pico,
 * oferecendo controle direto e de baixo nível sobre o display OLED via comandos SSD1306 padronizados.
 *
 * Ideal para aplicações que exigem exibição de texto, gráficos simples e interface visual em sistemas com I²C.
 */


#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

void ssd1306_draw_utf8_string(uint8_t *ssd, int16_t x, int16_t y, const char *utf8_string);
void ssd1306_draw_utf8_multiline(uint8_t *ssd, int16_t x, int16_t y, const char *utf8_string);

#ifndef ssd1306_inc_h
#define ssd1306_inc_h

#define ssd1306_height 64 // Define a altura do display (32 pixels)
#define ssd1306_width 128 // Define a largura do display (128 pixels)

#define ssd1306_i2c_address _u(0x3C) // Define o endereço do i2c do display

#define ssd1306_i2c_clock 400 // Define o tempo do clock (pode ser aumentado)

// Comandos de configuração (endereços)
#define ssd1306_set_memory_mode _u(0x20)
#define ssd1306_set_column_address _u(0x21)
#define ssd1306_set_page_address _u(0x22)
#define ssd1306_set_horizontal_scroll _u(0x26)
#define ssd1306_set_scroll _u(0x2E)

#define ssd1306_set_display_start_line _u(0x40)

#define ssd1306_set_contrast _u(0x81)
#define ssd1306_set_charge_pump _u(0x8D)

#define ssd1306_set_segment_remap _u(0xA0)
#define ssd1306_set_entire_on _u(0xA4)
#define ssd1306_set_all_on _u(0xA5)
#define ssd1306_set_normal_display _u(0xA6)
#define ssd1306_set_inverse_display _u(0xA7)
#define ssd1306_set_mux_ratio _u(0xA8)
#define ssd1306_set_display _u(0xAE)
#define ssd1306_set_common_output_direction _u(0xC0)
#define ssd1306_set_common_output_direction_flip _u(0xC0)

#define ssd1306_set_display_offset _u(0xD3)
#define ssd1306_set_display_clock_divide_ratio _u(0xD5)
#define ssd1306_set_precharge _u(0xD9)
#define ssd1306_set_common_pin_configuration _u(0xDA)
#define ssd1306_set_vcomh_deselect_level _u(0xDB)

#define ssd1306_page_height _u(8)
#define ssd1306_n_pages (ssd1306_height / ssd1306_page_height)
#define ssd1306_buffer_length (ssd1306_n_pages * ssd1306_width)

#define ssd1306_write_mode _u(0xFE)
#define ssd1306_read_mode _u(0xFF)

struct render_area {
    uint8_t start_column;
    uint8_t end_column;
    uint8_t start_page;
    uint8_t end_page;

    int buffer_length;
};

typedef struct {
  uint8_t width, height, pages, address;
  i2c_inst_t * i2c_port;
  bool external_vcc;
  uint8_t *ram_buffer;
  size_t bufsize;
  uint8_t port_buffer[2];
} ssd1306_t;

#endif