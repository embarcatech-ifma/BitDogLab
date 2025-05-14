
## 🖥️ Exibição de Texto no Display OLED (SSD1306)

### Bibliotecas
```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
```
### Tem que ser adicionado no cmakelist as configurações abaixo:
```c
add_executable(lcd_oled_HelloWorld lcd_oled_HelloWorld.c ssd1306_i2c.c) 

 # Add any user requested libraries
target_link_libraries(nomedoarquivo 
hardware_adc  
hardware_i2c
 )
```

### Declaração de variaveis 
```c
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area;

```

### 1.Função de desenho

```c/ Função para desenhar texto com escala
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
```

### 🖼️ 2. Configuração da Área de Renderização

```c
frame_area.start_column = 0;
frame_area.end_column = ssd1306_width - 1;
frame_area.start_page = 0;
frame_area.end_page = ssd1306_n_pages - 1;
calculate_render_area_buffer_length(&frame_area);
```

---

### ✍️ 3. Exibição de Texto no Display

```c
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
```

### ✅ Resultado

O display OLED mostra:

![WhatsApp Image 2025-05-13 at 15 20 11](https://github.com/user-attachments/assets/c9a29799-c07e-45fd-860f-dc41a627e899)

Para mais detalhes, só abrir o arquivo .c
Todo o codigo comentado com os detalhes.
