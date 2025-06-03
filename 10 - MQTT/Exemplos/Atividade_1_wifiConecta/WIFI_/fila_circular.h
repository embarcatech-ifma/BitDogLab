/**
 * @file fila_circular.h
 * @brief Interface da fila circular protegida por mutex para comunicação entre núcleos.
 */
#include "configura_geral.h"

#ifndef FILA_CIRCULAR_H
#define FILA_CIRCULAR_H



typedef struct {
    uint16_t tentativa;
    uint16_t status;
} MensagemWiFi;

typedef struct {
    MensagemWiFi fila[TAM_FILA];
    int frente;
    int tras;
    int tamanho;
    mutex_t mutex;
} FilaCircular;

void fila_inicializar(FilaCircular *f);
bool fila_inserir(FilaCircular *f, MensagemWiFi m);
bool fila_remover(FilaCircular *f, MensagemWiFi *saida);
bool fila_vazia(FilaCircular *f);

#endif
