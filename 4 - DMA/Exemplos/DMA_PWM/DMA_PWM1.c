#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#define LED_PIN 12
#define PWM_WRAP 255
#define FADE_STEPS 256
#define REPEAT_FACTOR 45  // Repetições por valor
#define FADE_LEN (FADE_STEPS * 2 * REPEAT_FACTOR)  // Tamanho final: 512 * 45 = 23040

uint16_t fade_table[FADE_LEN];

volatile bool dma_reiniciar = false;

void dma_irq_handler() {
    dma_hw->ints0 = 1u << 0;  // Limpa interrupção do canal 0
    dma_reiniciar = true;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("🚀 Iniciando PWM com DMA (Efeito de Respiração Suave)\n");

    // Preenche a tabela com valores repetidos
    for (int i = 0; i < FADE_STEPS; i++) {
        uint16_t val_up = i;
        uint16_t val_down = PWM_WRAP - i;

        for (int j = 0; j < REPEAT_FACTOR; j++) {
            fade_table[i * REPEAT_FACTOR + j] = val_up;
            fade_table[(FADE_STEPS + i) * REPEAT_FACTOR + j] = val_down;
        }
    }

    // Configura GPIO como PWM
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(LED_PIN);
    uint chan = pwm_gpio_to_channel(LED_PIN);

    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_clkdiv(slice, 64.0f); // Frequência ~7.6kHz
    pwm_set_enabled(slice, true);

    // Configura DMA
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, true);
    channel_config_set_write_increment(&cfg, false);
    channel_config_set_dreq(&cfg, DREQ_PWM_WRAP0 + slice);

    uint32_t pwm_dest_reg = (chan == PWM_CHAN_A)
        ? (uintptr_t)&pwm_hw->slice[slice].cc
        : (uintptr_t)&pwm_hw->slice[slice].cc + 2;

    dma_channel_configure(
        dma_chan, &cfg,
        (void *)pwm_dest_reg,
        fade_table,
        FADE_LEN,
        false // Não inicia ainda
    );

    // IRQ para reiniciar DMA
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_set_irq0_enabled(dma_chan, true);

    dma_channel_start(dma_chan);

    while (true) {
        if (dma_reiniciar) {
            dma_reiniciar = false;

            dma_channel_set_read_addr(dma_chan, fade_table, false);
            dma_channel_set_trans_count(dma_chan, FADE_LEN, true);
        }

        tight_loop_contents();
    }

    return 0;
}
