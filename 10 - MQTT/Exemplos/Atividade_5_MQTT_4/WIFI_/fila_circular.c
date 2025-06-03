/**
 * @file fila_circular.c
 * @brief Implementação da fila circular com proteção por mutex.
 */

#include "fila_circular.h"

void fila_inicializar(FilaCircular *f) {
    f->frente = 0;
    f->tras = -1;
    f->tamanho = 0;
    mutex_init(&f->mutex);
}

bool fila_inserir(FilaCircular *f, MensagemWiFi m) {
    bool sucesso = false;
    mutex_enter_blocking(&f->mutex);

    if (f->tamanho < TAM_FILA) {
        f->tras = (f->tras + 1) % TAM_FILA;
        f->fila[f->tras] = m;
        f->tamanho++;
        sucesso = true;
    }

    mutex_exit(&f->mutex);
    return sucesso;
}

bool fila_remover(FilaCircular *f, MensagemWiFi *saida) {
    bool sucesso = false;
    mutex_enter_blocking(&f->mutex);

    if (f->tamanho > 0) {
        *saida = f->fila[f->frente];
        f->frente = (f->frente + 1) % TAM_FILA;
        f->tamanho--;
        sucesso = true;
    }

    mutex_exit(&f->mutex);
    return sucesso;
}

bool fila_vazia(FilaCircular *f) {
    return f->tamanho == 0;
}
