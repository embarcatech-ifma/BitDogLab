/**
 * @file ssd1306.h
 * @brief Interface unificada para controle de displays OLED com controlador SSD1306 via I²C.
 *
 * Este cabeçalho reúne as declarações das funções necessárias para configurar, enviar comandos,
 * manipular pixels, desenhar formas, caracteres e bitmaps em displays OLED baseados no controlador SSD1306.
 *
 * As funções abrangem:
 * - Inicialização do display (`ssd1306_init`, `ssd1306_config`, `ssd1306_init_bm`)
 * - Controle e envio de comandos (`ssd1306_send_command`, `ssd1306_command`, `ssd1306_scroll`)
 * - Envio de dados gráficos (`ssd1306_send_buffer`, `ssd1306_send_data`)
 * - Manipulação gráfica de alto nível (`ssd1306_set_pixel`, `ssd1306_draw_line`, `ssd1306_draw_char`, `ssd1306_draw_string`, `ssd1306_draw_bitmap`)
 * - Renderização direta de regiões de memória (`render_on_display`, `calculate_render_area_buffer_length`)
 *
 * Este módulo assume o uso de um barramento I²C para comunicação com o display e depende da estrutura `ssd1306_t`
 * definida em arquivos complementares, como `ssd1306_i2c.h`.
 *
 * Ideal para aplicações que demandam visualização em microcontroladores com suporte gráfico básico,
 * como o Raspberry Pi Pico W.
 */


#include "ssd1306_i2c.h"
extern void calculate_render_area_buffer_length(struct render_area *area);
extern void ssd1306_send_command(uint8_t cmd);
extern void ssd1306_send_command_list(uint8_t *ssd, int number);
extern void ssd1306_send_buffer(uint8_t ssd[], int buffer_length);
extern void ssd1306_init();
extern void ssd1306_scroll(bool set);
extern void render_on_display(uint8_t *ssd, struct render_area *area);
extern void ssd1306_set_pixel(uint8_t *ssd, int x, int y, bool set);
extern void ssd1306_draw_line(uint8_t *ssd, int x_0, int y_0, int x_1, int y_1, bool set);
extern void ssd1306_draw_char(uint8_t *ssd, int16_t x, int16_t y, uint8_t character);
extern void ssd1306_draw_string(uint8_t *ssd, int16_t x, int16_t y, char *string);
extern void ssd1306_command(ssd1306_t *ssd, uint8_t command);
extern void ssd1306_config(ssd1306_t *ssd);
extern void ssd1306_init_bm(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);
extern void ssd1306_send_data(ssd1306_t *ssd);
extern void ssd1306_draw_bitmap(ssd1306_t *ssd, const uint8_t *bitmap);