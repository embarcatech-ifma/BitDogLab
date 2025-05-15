#ifndef TAREFA5_MOVEL_GPIO_DEADLINE_H
#define TAREFA5_MOVEL_GPIO_DEADLINE_H

/**
 * @brief Executa análise de tendência com média móvel por blocos.
 *        Atualiza o LED RGB via GPIO conforme a tendência.
 *        Controle rigoroso de tempo com busy_wait_until().
 *
 * @param nova_media Valor médio da temperatura no ciclo atual
 */
void tarefa5_movel_com_deadline(float nova_media, absolute_time_t inicio_ciclo);

#endif
