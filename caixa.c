#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "caixa.h"
#include "pessoa.h"

void iniciar_caixa(Caixa *caixa)
{
    caixa->ocupado = 0;
    caixa->tamanho_fila = 0;
    caixa->ordem = 0;

    pthread_mutex_init(&caixa->mutex, NULL);

    for(int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_cond_init(&caixa->cond[i], NULL);
        caixa->fila[i] = NULL;
    }
}

void destruir_caixa(Caixa *caixa)
{
    pthread_mutex_destroy(&caixa->mutex);

    for(int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_cond_destroy(&caixa->cond[i]);
    }
}

void mostrar_fila_formatada(Caixa *caixa)
{
    printf("  {fila:");
    for (int i = 0; i < caixa->tamanho_fila; i++)
    {
        if (caixa->fila[i] != NULL)
        {
            printf("%c", caixa->fila[i]->nome[0]);
        }
    }
    printf("}\n");
}

int vez_pessoa(Caixa *caixa, Pessoa *pessoa)
{
    for (int i = 0; i < caixa->tamanho_fila; i++)
    {
        if (caixa->fila[i] != NULL && caixa->fila[i] != pessoa)
        {
            if (caixa->fila[i]->prioridade_atual > pessoa->prioridade_atual)
            {
                return 0;
            }
            if (caixa->fila[i]->prioridade_atual == pessoa->prioridade_atual &&
                caixa->fila[i]->ordem_chegada < pessoa->ordem_chegada)
            {
                return 0;
            }
        }
    }
    return 1;
}

void esperar_caixa(Caixa *caixa, Pessoa *pessoa)
{
    pthread_mutex_lock(&caixa->mutex);

    caixa->ordem++;
    pessoa->ordem_chegada = caixa->ordem;

    caixa->fila[caixa->tamanho_fila] = pessoa;
    caixa->tamanho_fila++;

    printf("%s está na fila do caixa", pessoa->nome);
    mostrar_fila_formatada(caixa);

    while (caixa->ocupado || !vez_pessoa(caixa, pessoa))
    {
        pthread_cond_wait(&caixa->cond[pessoa->id], &caixa->mutex);
    }

    caixa->ocupado = 1;

    int indice_pessoa = -1;
    for (int i = 0; i < caixa->tamanho_fila; i++)
    {
        if (caixa->fila[i] == pessoa)
        {
            indice_pessoa = i;
            break;
        }
    }
    if (indice_pessoa != -1)
    {
        for (int i = indice_pessoa; i < caixa->tamanho_fila - 1; i++)
        {
            caixa->fila[i] = caixa->fila[i + 1];
        }
        caixa->tamanho_fila--;
        caixa->fila[caixa->tamanho_fila] = NULL;
    }

    pessoa->frustracoes = 0;
    pessoa->prioridade_atual = pessoa->prioridade_inicial;

    printf("%s está sendo atendido(a)", pessoa->nome);
    mostrar_fila_formatada(caixa);

    pthread_mutex_unlock(&caixa->mutex);
}

void liberar_caixa(Caixa *caixa, Pessoa *pessoa)
{
    pthread_mutex_lock(&caixa->mutex);

    caixa->ocupado = 0;

    printf("%s vai para casa", pessoa->nome);
    mostrar_fila_formatada(caixa);

    // Acorda as threads que estão aguardando na fila para reavaliarem sua vez
    if (caixa->tamanho_fila > 0)
    {
        for (int i = 0; i < caixa->tamanho_fila; i++) 
        {
            if (caixa->fila[i] != NULL) 
            {
                pthread_cond_signal(&caixa->cond[caixa->fila[i]->id]);
            }
        }
    }

    pthread_mutex_unlock(&caixa->mutex);
}
