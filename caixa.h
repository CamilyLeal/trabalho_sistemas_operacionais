#ifndef CAIXA_H
#define CAIXA_H

#include <pthread.h>
#include "pessoa.h"

#define MAX_PESSOAS 8

typedef struct {
    pthread_mutex_t mutex; //apenas uma thread por vez
    pthread_cond_t cond[MAX_PESSOAS]; //variável de condição
    Pessoa *fila[MAX_PESSOAS]; //fila de espera
    int ocupado; //0 ou 1
    int tamanho_fila;
    int ordem;
} Caixa;

void iniciar_caixa(Caixa *caixa);
void destruir_caixa(Caixa *caixa);
#endif