#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "tarefa5_movel_gpio_deadline.h"
#include "funcao_do_projeto.h"
#include "tarefa3_tendencia.h"

#define TAM_BLOCO 50
#define LIMIAR_TENDENCIA 0.05f
#define INTERVALO_MS 1000  // 1 segundo por ciclo

static float buffer[TAM_BLOCO];
static int indice = 0;
static float media_anterior = 0.0f;

void tarefa5_movel_com_deadline(float nova_media) {
    static bool primeira_execucao = true;
    static absolute_time_t proximo_ciclo;
    static absolute_time_t tempo_anterior;

    // Inicializa o primeiro deadline
    if (primeira_execucao) {
        proximo_ciclo = get_absolute_time();
        tempo_anterior = proximo_ciclo;
        primeira_execucao = false;
    }

    buffer[indice++] = nova_media;
    printf("[Bloco] Progresso: %d / %d\n", indice, TAM_BLOCO);

    if (indice >= TAM_BLOCO) {
        float soma = 0;
        for (int i = 0; i < TAM_BLOCO; i++) {
            soma += buffer[i];
        }
        float media_atual = soma / TAM_BLOCO;

        float delta = media_atual - media_anterior;
        tendencia_t tendencia_detectada = TENDENCIA_ESTAVEL;

        if (media_anterior != 0.0f) {
            if (delta > LIMIAR_TENDENCIA) {
                gpio_put(LED_VERMELHO, 1);
                gpio_put(LED_VERDE, 0);
                gpio_put(LED_AZUL, 0);
                tendencia_detectada = TENDENCIA_SUBINDO;
            } else if (delta < -LIMIAR_TENDENCIA) {
                gpio_put(LED_VERMELHO, 0);
                gpio_put(LED_VERDE, 0);
                gpio_put(LED_AZUL, 1);
                tendencia_detectada = TENDENCIA_CAINDO;
            } else {
                gpio_put(LED_VERMELHO, 0);
                gpio_put(LED_VERDE, 1);
                gpio_put(LED_AZUL, 0);
                tendencia_detectada = TENDENCIA_ESTAVEL;
            }

            printf("Bloco anterior: %.2f °C | Bloco atual: %.2f °C | Tendência: %s\n",
                   media_anterior, media_atual, tendencia_para_texto(tendencia_detectada));
        }

        media_anterior = media_atual;
        indice = 0;
    }

    // Espera até o próximo ciclo fixo
    busy_wait_until(proximo_ciclo);
    proximo_ciclo = delayed_by_ms(proximo_ciclo, INTERVALO_MS);

    // Monitor de tempo real
    absolute_time_t agora = get_absolute_time();
    int64_t delta_us = absolute_time_diff_us(tempo_anterior, agora);
    printf("[Monitor] Tempo entre execuções da T5: %.3f s\n", delta_us / 1e6);
    tempo_anterior = agora;
}
