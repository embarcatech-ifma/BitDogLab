/**
 * ------------------------------------------------------------
 *  Arquivo: setup.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Este módulo centraliza todas as configurações iniciais
 *      necessárias para o funcionamento do projeto, incluindo:
 *      
 *      - Inicialização do terminal USB (stdio)
 *      - Configuração do ADC e habilitação do sensor interno
 *      - Configuração do canal DMA para leitura da temperatura
 *      - Registro da interrupção do canal DMA 0
 *      - Inicialização do display OLED (SSD1306)
 *
 *      A função principal `setup()` deve ser chamada uma única
 *      vez no início do programa, geralmente logo no `main()`,
 *      para garantir que o sistema esteja corretamente preparado
 *      antes de iniciar o executor cíclico.
 *
 *  Relacionamento:
 *      - Define a configuração global `cfg_temp` para uso
 *        posterior na Tarefa 1 (tarefa1_temp.c)
 *      - Define os símbolos globais `ssd[]` e `area` usados na
 *        Tarefa 2 (tarefa2_display.c)
 *      - Utiliza o handler de interrupção definido em
 *        'irq_handlers.c'
 *
 *  
 *  *  Data: 11/05/2025
 * ------------------------------------------------------------
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "setup.h"
#include "irq_handlers.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "neopixel_driver.h"

// === Buffer de vídeo do OLED (tela de 128 x 64) ===
uint8_t ssd[ssd1306_buffer_length];

// === Área de renderização usada por render_on_display() ===
struct render_area area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

// === Configuração global do canal DMA 0 ===
dma_channel_config cfg_temp;

/**
 * @brief Realiza a configuração inicial do sistema.
 *
 * Esta função inicializa o terminal USB, ADC, sensor de temperatura,
 * canal DMA 0, interrupções e o display OLED.
 */
void setup() {
    // Inicializa a comunicação USB para printf()
    stdio_init_all();
    //while (!stdio_usb_connected()) sleep_ms(200);  // Aguarda conexão USB

    // Inicializa o ADC do RP2040 e habilita o sensor interno (canal 4)
    adc_init();
    adc_set_temp_sensor_enabled(true);

    // Configura o canal DMA 0 para transferir dados do ADC
    cfg_temp = dma_channel_get_default_config(DMA_TEMP_CHANNEL);
    channel_config_set_transfer_data_size(&cfg_temp, DMA_SIZE_16);  // 16 bits
    channel_config_set_read_increment(&cfg_temp, false);            // ADC FIFO fixo
    channel_config_set_write_increment(&cfg_temp, true);            // Buffer se move
    channel_config_set_dreq(&cfg_temp, DREQ_ADC);                   // dispara com ADC

    // Configura interrupção do canal DMA 0
    dma_channel_set_irq0_enabled(DMA_TEMP_CHANNEL, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler_temp);
    irq_set_enabled(DMA_IRQ_0, true);

    // Inicializa o display OLED SSD1306 via I2C
    i2c_init(i2c1, 400 * 1000);  // <---I2C primeiro
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);

    ssd1306_init();             // <---Depois do I2C estar pronto
    calculate_render_area_buffer_length(&area);

    // Inicializa NeoPixel (Matriz RGB)
    npInit(LED_PIN);  // substitua LED_PIN pelo valor real, ex: 7
}