/**
 * @file setup_wifi.c
 * @brief Configuração inicial do sistema: barramento I²C, display OLED e conexão Wi-Fi no Raspberry Pi Pico W.
 *
 * Este módulo contém a função `setup()`, responsável por inicializar o barramento I²C, configurar o display OLED
 * e iniciar a conexão Wi-Fi com feedback visual ao usuário. Ele deve ser chamado uma única vez no início da aplicação,
 * normalmente dentro da função `main()`.
 *
 * A sequência de inicialização garante que os periféricos estejam prontos antes de qualquer operação de rede
 * ou exibição de informações no display.
 *
 * Dependências:
 * - `wifi_status.h`: fornece as definições de pinos, buffers gráficos e funções de conexão.
 * - Biblioteca `ssd1306` e `oled_utils` para controle do display.
 * - Biblioteca `cyw43_arch` para conexão Wi-Fi.
 */

#include "configura_geral.h" // Inclui configurações de pinos, buffer OLED e função de conexão
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"           // ← necessário para ssd1306_init e calculate_render_area_buffer_length
#include "oled_utils.h"        // ← necessário para oled_clear
#include "setup_oled.h"  

/**
 * @brief Função principal de configuração do sistema.
 *
 * Responsável por:
 * - Inicializar o barramento I²C nos pinos definidos (SDA, SCL).
 * - Configurar os pinos com resistores de pull-up.
 * - Inicializar o display OLED (SSD1306).
 * - Definir a área de renderização do display.
 * - Limpar o conteúdo do display.
 * - Iniciar o processo de conexão Wi-Fi com exibição de status.
 */
void setup_init_oled(void) {
    // Inicializa o barramento I²C na porta i2c1 com frequência de 400 kHz
    i2c_init(i2c1, 400 * 1000);

    // Define os pinos SDA e SCL como função I²C
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    // Ativa resistores de pull-up nos pinos I²C
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Inicializa o display OLED com o controlador SSD1306
    ssd1306_init();

    // Define a área de renderização para cobrir toda a tela
    area.start_column = 0;
    area.end_column = ssd1306_width - 1;
    area.start_page = 0;
    area.end_page = ssd1306_n_pages - 1;

    // Calcula o tamanho do buffer necessário para essa área
    calculate_render_area_buffer_length(&area);

    // Limpa completamente o display
    oled_clear(buffer_oled, &area);

}
