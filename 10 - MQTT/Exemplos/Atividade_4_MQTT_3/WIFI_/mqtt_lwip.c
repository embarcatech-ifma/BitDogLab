/**
 * @file mqtt_lwip.c
 * @brief Implementação do cliente MQTT usando a pilha lwIP no Raspberry Pi Pico W.
 *
 * Este módulo gerencia a conexão com o broker MQTT e a publicação/assinatura de mensagens.
 * Utiliza a pilha lwIP e a API MQTT (lwip/apps/mqtt.h) para comunicação TCP/IP.
 *
 * Principais responsabilidades:
 * - Criar e configurar o cliente MQTT.
 * - Conectar-se ao broker definido via IP.
 * - Assinar múltiplos tópicos e registrar callbacks de entrada.
 * - Publicar mensagens de forma segura, evitando congestionamento de envio.
 * - Notificar o núcleo 0 via FIFO sobre resultados de publicação.
 */

#include <stdio.h>
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include "configura_geral.h"
#include "display_utils.h"
#include "mqtt_lwip.h"

// ========================
// VARIÁVEIS GLOBAIS INTERNAS
// ========================

bool publicar_online = false;


/**
 * @brief Armazena o nome do tópico recebido no último pacote MQTT.
 */
static char topico_recebido[64] = {0};

/**
 * @brief Ponteiro para o cliente MQTT.
 *
 * Criado com mqtt_client_new() e usado em todas as operações MQTT.
 */
static mqtt_client_t *client;

/**
 * @brief Estrutura com informações do cliente MQTT (ID, credenciais).
 *
 * Inicializada com client_id "pico_lwip". Pode ser expandida para login/senha.
 */
static struct mqtt_connect_client_info_t ci;

/**
 * @brief Flag de controle para impedir publicações simultâneas.
 */
static bool publicacao_em_andamento = false;

// ========================
// CALLBACKS DE ASSINATURA E DADOS
// ========================

/**
 * @brief Callback chamado ao identificar o tópico de uma nova mensagem recebida.
 *
 * Armazena o nome do tópico para uso posterior no callback de dados.
 */
static void mqtt_mensagem_cb(void *arg, const char *topic, u32_t tot_len) {
    strncpy(topico_recebido, topic, sizeof(topico_recebido) - 1);
    topico_recebido[sizeof(topico_recebido) - 1] = '\0';
}

/**
 * @brief Callback chamado com os dados de uma mensagem recebida.
 *
 * Se o tópico for `TOPICO_CONFIG_INTERVALO`, interpreta o payload como um número inteiro
 * e envia via FIFO ao núcleo 0 para atualizar o intervalo do PING.
 */
static void mqtt_dados_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    if (strncmp(topico_recebido, TOPICO_CONFIG_INTERVALO, strlen(TOPICO_CONFIG_INTERVALO)) == 0) {
        char buffer[16] = {0};
        memcpy(buffer, data, len < sizeof(buffer) - 1 ? len : sizeof(buffer) - 1);
        uint32_t novo_valor = (uint32_t) atoi(buffer);

        if (novo_valor >= 1000 && novo_valor <= 60000) {
            multicore_fifo_push_blocking((0xABCD << 16) | (novo_valor & 0xFFFF));
        }
    }
}

/**
 * @brief Callback chamado após tentativa de assinatura de um tópico.
 *
 * Exibe uma mensagem de sucesso ou falha no terminal.
 */
static void mqtt_sub_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("[MQTT] Tópico assinado com sucesso.\n");
    } else {
        printf("[MQTT] Falha ao assinar tópico. Código: %d\n", result);
    }
}

// ========================
// CALLBACKS DE CONEXÃO E PUBLICAÇÃO
// ========================

/**
 * @brief Callback chamado após tentativa de conexão com o broker MQTT.
 *
 * Se a conexão for aceita, registra os callbacks de entrada e assina os tópicos necessários.
 */
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        exibir_status_mqtt("CONECTADO");

        mqtt_set_inpub_callback(client, mqtt_mensagem_cb, mqtt_dados_cb, NULL);

        mqtt_subscribe(client, TOPICO_CONFIG_INTERVALO, 0, mqtt_sub_cb, NULL);
        mqtt_subscribe(client, TOPICO_COMANDO_LED,      0, mqtt_sub_cb, NULL);
        mqtt_subscribe(client, TOPICO_COMANDO_RGB,      0, mqtt_sub_cb, NULL);
        mqtt_subscribe(client, TOPICO_MENSAGEM_OLED,    0, mqtt_sub_cb, NULL);

        //publicar_mensagem_mqtt(TOPICO_ONLINE, "Pico W online");
        publicar_online = true;
    } else {
        exibir_status_mqtt("FALHA");
    }
}

/**
 * @brief Callback chamado após o término de uma publicação MQTT.
 *
 * Envia via FIFO para o núcleo 0 um status de sucesso (0) ou erro (1),
 * com código de controle 0x9999.
 */
static void mqtt_pub_cb(void *arg, err_t result) {
    publicacao_em_andamento = false;

    printf("[MQTT] Publicação finalizada: %s\n", result == ERR_OK ? "OK" : "ERRO");

    uint16_t status = (result == ERR_OK) ? 0 : 1;
    uint32_t pacote = ((0x9999 << 16) | status);
    multicore_fifo_push_blocking(pacote);
}

// ========================
// FUNÇÕES PRINCIPAIS
// ========================

/**
 * @brief Inicializa e conecta o cliente MQTT ao broker.
 *
 * Converte o IP do broker, instancia o cliente e realiza a conexão
 * com os parâmetros configurados.
 */
void iniciar_mqtt_cliente() {
    ip_addr_t broker_ip;

    if (!ip4addr_aton(MQTT_BROKER_IP, &broker_ip)) {
        printf("Endereço IP do broker inválido: %s\n", MQTT_BROKER_IP);
        return;
    }

    client = mqtt_client_new();
    if (!client) {
        printf("Erro ao criar cliente MQTT\n");
        return;
    }

    memset(&ci, 0, sizeof(ci));
    ci.client_id = "pico_lwip";

    mqtt_client_connect(client, &broker_ip, MQTT_BROKER_PORT, mqtt_connection_cb, NULL, &ci);
}

/**
 * @brief Publica uma mensagem MQTT de forma segura.
 *
 * Verifica se o cliente está conectado e se nenhuma publicação anterior está pendente.
 * Usa `publicacao_em_andamento` como trava para evitar sobrecarga da fila TCP.
 *
 * @param topico  Nome do tópico a ser publicado.
 * @param mensagem  Conteúdo textual a ser enviado.
 */
void publicar_mensagem_mqtt(const char *topico, const char *mensagem) {
    if (!client) {
        printf("[MQTT] Cliente NULL\n");
        exibir_status_mqtt("CLIENTE NULL");
        return;
    }

    if (!mqtt_client_is_connected(client)) {
        printf("[MQTT] MQTT desconectado. Ignorando publicação.\n");
        exibir_status_mqtt("DESCONECTADO");
        return;
    }

    if (publicacao_em_andamento) {
        printf("[MQTT] Publicação anterior ainda não finalizada. Ignorado.\n");
        return;
    }

    err_t err = mqtt_publish(client,
                             topico,
                             mensagem,
                             strlen(mensagem),
                             0,    // QoS
                             0,    // retain
                             mqtt_pub_cb,
                             NULL);

    if (err != ERR_OK) {
        printf("[MQTT] Erro ao publicar: %d\n", err);
        exibir_status_mqtt("PUB ERRO");
    } else {
        printf("[MQTT] Publicando: \"%s\" em \"%s\"\n", mensagem, topico);
        publicacao_em_andamento = true;
    }
}

/**
 * @brief Função reservada para manutenções futuras no cliente MQTT.
 *
 * Pode ser usada para reconexões, manutenção ativa ou ping manual.
 */
void mqtt_loop() {
    // Espaço reservado para verificações futuras
}


void publicar_online_retain(void) {
    if (!client || !mqtt_client_is_connected(client)) {
        printf("[MQTT] Cliente não conectado. Não foi possível publicar 'Pico W online'.\n");
        return;
    }

    const char *mensagem = "Pico W online";

    err_t err = mqtt_publish(client,
                             TOPICO_ONLINE,
                             mensagem,
                             strlen(mensagem),
                             0,  // QoS 0
                             1,  // retain = 1 ✅
                             mqtt_pub_cb,
                             NULL);

    if (err != ERR_OK) {
        printf("[MQTT] Falha ao publicar 'Pico W online': código %d\n", err);
        exibir_status_mqtt("PUB ERRO");
    } else {
        printf("[MQTT] 'Pico W online' publicado com retain.\n");
    }
}


bool cliente_mqtt_ativo(void) {
    return client && mqtt_client_is_connected(client);
}
