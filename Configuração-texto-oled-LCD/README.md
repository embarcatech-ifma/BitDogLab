
## 🖥️ Exibição de Texto no Display OLED (SSD1306)


### ⚙️ 1. Inicialização do I2C e do Display

```c
i2c_init(i2c1, ssd1306_i2c_clock * 1000);
gpio_set_function(14, GPIO_FUNC_I2C); // SDA
gpio_set_function(15, GPIO_FUNC_I2C); // SCL
gpio_pull_up(14);
gpio_pull_up(15);
ssd1306_init();
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
uint8_t ssd[ssd1306_buffer_length];
memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer

ssd1306_draw_string_scaled(ssd, 20, 20, "Hello Word", 2);

render_on_display(ssd, &frame_area);
```

### ✅ Resultado

O display OLED mostra mensagens como:

- `Hello Word`


