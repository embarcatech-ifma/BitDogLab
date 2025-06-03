/**
 * @file mqtt_lwip.c
 * @brief Implementação do cliente MQTT usando a pilha lwIP no Raspberry Pi Pico W.
 *
 * Este módulo gerencia a conexão com o broker MQTT e a publicação de mensagens.
 * Ele utiliza a pilha lwIP e sua API MQTT (lwip/apps/mqtt.h) para comunicação TCP/IP.
 *
 * As principais funcionalidades incluem:
 * - Criação e configuração de um cliente MQTT (`mqtt_client_new`);
 * - Conexão com o broker definido via IP (`MQTT_BROKER_IP`);
 * - Callback para conexão bem-sucedida ou falha (`mqtt_connection_cb`);
 * - Publicação de mensagens (`publicar_mensagem_mqtt`);
 * - Callback de confirmação da publicação (`mqtt_pub_cb`);
 * - Uma função vazia `mqtt_loop()` preparada para expansões futuras (ex: manutenção da conexão).
 *
 * Este código é ativado pelo núcleo 0, após a obtenção de um IP válido.
 */

#include <stdio.h>
#include "lwip/apps/mqtt.h"     // API MQTT da lwIP
#include "lwip/ip_addr.h"       // Manipulação de endereços IP
#include "configura_geral.h"    // Define constantes como TOPICO, MQTT_BROKER_IP, MQTT_BROKER_PORT
#include "display_utils.h"      // exibir_status_mqtt() e funções de feedback visual

// ========================
// VARIÁVEIS GLOBAIS INTERNAS
// ========================

/**
 * @brief Ponteiro para o cliente MQTT.
 *
 * É criado em tempo de execução por `mqtt_client_new()` e usado para gerenciar a conexão.
 */
static mqtt_client_t *client;

/**
 * @brief Estrutura com informações do cliente MQTT (ID, credenciais, etc.).
 *
 * Inicializada com client_id "pico_lwip" — pode ser expandida para autenticação.
 */
static struct mqtt_connect_client_info_t ci;

// ========================
// DECLARAÇÕES
// ========================

/**
 * @brief Declaração antecipada da função de publicação, usada no callback de conexão.
 */
void publicar_mensagem_mqtt(const char *mensagem);


// ========================
// CALLBACKS DO MQTT
// ========================

/**
 * @brief Callback chamado após tentativa de conexão com o broker MQTT.
 *
 * Verifica se a conexão foi aceita. Em caso de sucesso, exibe o status e publica uma mensagem de teste.
 *
 * @param client ponteiro para o cliente MQTT
 * @param arg argumento opcional (não utilizado aqui)
 * @param status status da tentativa de conexão
 */
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED) {
        exibir_status_mqtt("CONECTADO");
        publicar_mensagem_mqtt("Pico W online");
    } else {
        exibir_status_mqtt("FALHA");
    }
}

/**
 * @brief Callback chamado após a tentativa de publicação de uma mensagem.
 *
 * Confirma se a mensagem foi publicada com sucesso ou informa erro.
 *
 * @param arg argumento opcional (não utilizado aqui)
 * @param result código de erro do tipo `err_t`
 */
void mqtt_pub_cb(void *arg, err_t result)
{
    if (result == ERR_OK) {
        printf("Mensagem publicada com sucesso no tópico: %s\n", TOPICO);
    } else {
        printf("Erro ao publicar mensagem! Código: %d\n", result);
    }
}


// ========================
// FUNÇÕES PRINCIPAIS
// ========================

/**
 * @brief Inicializa e conecta o cliente MQTT ao broker.
 *
 * A função converte o IP do broker (definido em `configura_geral.h`) e tenta se conectar à porta definida.
 * Em caso de erro, imprime mensagens no terminal e não prossegue.
 */
void iniciar_mqtt_cliente()
{
    ip_addr_t broker_ip;

    // Converte o IP textual para estrutura lwIP
    if (!ip4addr_aton(MQTT_BROKER_IP, &broker_ip)) {
        printf("Endereço IP do broker inválido: %s\n", MQTT_BROKER_IP);
        return;
    }

    // Cria o cliente MQTT
    client = mqtt_client_new();
    if (!client) {
        printf("Erro ao criar cliente MQTT\n");
        return;
    }

    // Limpa e configura a estrutura de informações do cliente
    memset(&ci, 0, sizeof(ci));
    ci.client_id = "pico_lwip";  // Nome que o broker verá

    // Conecta ao broker com callback de resultado
    mqtt_client_connect(client, &broker_ip, MQTT_BROKER_PORT, mqtt_connection_cb, NULL, &ci);
}

/**
 * @brief Publica uma mensagem no tópico definido.
 *
 * Antes de enviar, verifica se o cliente está conectado.
 *
 * @param mensagem texto a ser publicado no tópico MQTT.
 */
void publicar_mensagem_mqtt(const char *mensagem)
{
    // Garante que o cliente esteja conectado
    if (!client || !mqtt_client_is_connected(client)) {
        printf("Cliente MQTT não está conectado.\n");
        return;
    }

    // Publica a mensagem no tópico definido
    err_t err = mqtt_publish(client,
                             TOPICO,               // Tópico
                             mensagem,             // Mensagem
                             strlen(mensagem),     // Tamanho
                             0,                    // QoS 0
                             0,                    // Retain: falso
                             mqtt_pub_cb,          // Callback de confirmação
                             NULL);                // Argumento opcional

    // Verifica erro na publicação
    if (err != ERR_OK) {
        printf("Erro ao tentar publicar: %d\n", err);
    }
}

/**
 * @brief Função reservada para uso futuro (manutenção da conexão MQTT).
 *
 * Pode ser usada no futuro para reenvio, ping ou reconexão manual.
 */
void mqtt_loop() {
    // Espaço reservado para verificações futuras
}
