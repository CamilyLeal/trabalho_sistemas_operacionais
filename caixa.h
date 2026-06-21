#ifndef CAIXA_H
#define CAIXA_H

#include <pthread.h>
#include "pessoa.h"

#define MAX_PESSOAS 8

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond[MAX_PESSOAS]; // uma cond por pessoa, pra sinalizar só quem precisa
    Pessoa *fila[MAX_PESSOAS];
    int ocupado;
    int tamanho_fila;
    int ordem; 
    Pessoa *vitima_forcada; //vitima_forcada é setada pelo gerente quando detecta deadlock.
    
} Caixa;

void iniciar_caixa(Caixa *caixa);
void destruir_caixa(Caixa *caixa);
void esperar_caixa(Caixa *caixa, Pessoa *pessoa);
void liberar_caixa(Caixa *caixa, Pessoa *pessoa);
void mostrar_fila_formatada(Caixa *caixa);
int vez_pessoa(Caixa *caixa, Pessoa *pessoa);
int tem_precedencia(Prioridade a, Prioridade b);

#endif
