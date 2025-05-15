/**
 * ------------------------------------------------------------
 *  Arquivo: tarefa5_movel_gpio_deadline.c
 *  Projeto: TempCycleDMA / TrendWatch
 * ------------------------------------------------------------
 *  Descrição geral:
 *      Esta tarefa executa uma análise térmica com média móvel
 *      em blocos (TAM_BLOCO = 50), verificando a tendência da
 *      temperatura (subindo, caindo, estável).
 *
 *      A cada novo valor médio (vindo da Tarefa 1), ele é armazenado.
 *      Ao completar o bloco, calcula-se a média do bloco atual e
 *      compara-se com o bloco anterior:
 *         - Se subiu → LED vermelho (GPIO)
 *         - Se caiu  → LED azul (GPIO)
 *         - Se manteve → LED verde (GPIO)
 *
 *      A grande inovação aqui é o controle de tempo absoluto.
 *      A tarefa calcula o tempo restante após as Tarefas 1 a 4
 *      e espera exatamente o necessário para que o ciclo completo
 *      dure sempre 1000 ms (1 segundo).
 *
 *      Isso garante um sistema determinístico e sincronizado,
 *      como um relógio embarcado, sem acúmulo de atrasos.
 *
 *  Conceitos didáticos envolvidos:
 *      - Média móvel por blocos (filtro por estabilidade)
 *      - Análise de tendência térmica
 *      - Controle de LED RGB discreto via GPIO
 *      - Precisão de tempo com busy_wait_until()
 *      - Deadline ajustado conforme tempo das tarefas anteriores
 *
 *  
 *  Data: 2024
 * ------------------------------------------------------------
 */

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "tarefa5_movel_gpio_deadline.h"
#include "funcao_do_projeto.h"
#include "tarefa3_tendencia.h"

#define TAM_BLOCO 50
#define LIMIAR_TENDENCIA 0.05f
#define INTERVALO_US 1000000  // 1 segundo em microssegundos

static float buffer[TAM_BLOCO];
static int indice = 0;
static float media_anterior = 0.0f;

void tarefa5_movel_com_deadline(float nova_media, absolute_time_t inicio_ciclo) {
    static absolute_time_t tempo_anterior;
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

    // Cálculo do tempo restante com base no ciclo iniciado no main
    int64_t tempo_gasto_us = absolute_time_diff_us(inicio_ciclo, get_absolute_time());
    int64_t tempo_restante_us = INTERVALO_US - tempo_gasto_us;

    if (tempo_restante_us > 0) {
        absolute_time_t proximo_ciclo = delayed_by_us(get_absolute_time(), tempo_restante_us);
        busy_wait_until(proximo_ciclo);
    }

    // Monitor de tempo real
    absolute_time_t agora = get_absolute_time();
    int64_t delta_us = absolute_time_diff_us(tempo_anterior, agora);
    printf("[Monitor] Tempo entre execuções da Tarefa 5: %.3f s\n", delta_us / 1e6);
    tempo_anterior = agora;
}
