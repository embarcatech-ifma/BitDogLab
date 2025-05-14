#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area;

// Função para desenhar texto com escala
void ssd1306_draw_string_scaled(uint8_t *buffer, int x, int y, const char *text, int scale) {
    while (*text) {
        for (int dx = 0; dx < scale; dx++) {
            for (int dy = 0; dy < scale; dy++) {
                ssd1306_draw_char(buffer, x + dx, y + dy, *text);
            }
        }
        x += 6 * scale;
        text++;
    }
}

int main() {
    stdio_init_all();

    // Inicializa I2C e display
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();

    // Define área de renderização
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);

    // Limpa buffer
    memset(ssd, 0, ssd1306_buffer_length);

    // Define escala
    int scale = 2;

    // Calcula posição horizontal centralizada para ambas as palavras
    const char *linha1 = "Hello";
    const char *linha2 = "World";

    int largura_hello = strlen(linha1) * 6 * scale;
    int largura_world = strlen(linha2) * 6 * scale;

    int pos_x_hello = (ssd1306_width - largura_hello) / 2;
    int pos_x_world = (ssd1306_width - largura_world) / 2;

    // Define posições Y
    int pos_y_hello = 10;
    int pos_y_world = 35;

    // Desenha ambas as palavras
    ssd1306_draw_string_scaled(ssd, pos_x_hello, pos_y_hello, linha1, scale);
    ssd1306_draw_string_scaled(ssd, pos_x_world, pos_y_world, linha2, scale);

    // Renderiza no display
    render_on_display(ssd, &frame_area);

    while (1) {
        tight_loop_contents(); // Loop ocioso
    }

    return 0;
}
