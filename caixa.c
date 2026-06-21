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
    caixa->vitima_forcada = NULL;

    pthread_mutex_init(&caixa->mutex, NULL);

    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_cond_init(&caixa->cond[i], NULL);
        caixa->fila[i] = NULL;
    }
}

void destruir_caixa(Caixa *caixa)
{
    pthread_mutex_destroy(&caixa->mutex);

    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_cond_destroy(&caixa->cond[i]);
    }
}

void mostrar_fila_formatada(Caixa *caixa)
{
    printf(" {fila:");
    for (int i = 0; i < caixa->tamanho_fila; i++)
    {
        if (caixa->fila[i] != NULL)
        {
            printf("%c", caixa->fila[i]->nome[0]);
        }
    }
    printf("}\n");
}
/* 
 * Grávida > Idoso > Deficiente > Grávida 
 * Retorna 1 se 'a' tem precedência sobre 'b'. */
int tem_precedencia(Prioridade a, Prioridade b)
{
    if (a == COMUM)
        return 0;
    if (b == COMUM)
        return 1;
    if (a == b)
        return 0;

    switch (a)
    {
    case GRAVIDA_OU_CRIANCA_COLO:
        return b == IDOSO;
    case IDOSO:
        return b == DEFICIENTE;
    case DEFICIENTE:
        return b == GRAVIDA_OU_CRIANCA_COLO;
    default:
        return 0;
    }
}

/*  Se o gerente forçou alguém (vitima_forcada), só essa pessoa passa.
 * Caso contrário, verifica se tem alguém na fila com prioridade maior, ou mesma prioridade mas que chegou antes. */
int vez_pessoa(Caixa *caixa, Pessoa *pessoa)
{
    if (caixa->vitima_forcada == pessoa)
        return 1;

    if (caixa->vitima_forcada != NULL)
        return 0;

    for (int i = 0; i < caixa->tamanho_fila; i++)
    {
        if (caixa->fila[i] != NULL && caixa->fila[i] != pessoa)
        {
            if (tem_precedencia(caixa->fila[i]->prioridade_atual, pessoa->prioridade_atual))
                return 0;

            if (caixa->fila[i]->prioridade_atual == pessoa->prioridade_atual &&
                caixa->fila[i]->ordem_chegada < pessoa->ordem_chegada)
                return 0;
        }
    }
    return 1;
}

/* Entra na fila e bloqueia até ser a vez. */
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

    if (caixa->vitima_forcada == pessoa)
        caixa->vitima_forcada = NULL;

    caixa->ocupado = 1;

    // remove a pessoa da fila antes de imprimir "sendo atendido"
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
            caixa->fila[i] = caixa->fila[i + 1];
        caixa->tamanho_fila--;
        caixa->fila[caixa->tamanho_fila] = NULL;
    }

    // reseta prioridade e frustrações depois de ser atendido
    pessoa->frustracoes = 0;
    pessoa->prioridade_atual = pessoa->prioridade_inicial;

    printf("%s está sendo atendido(a)", pessoa->nome);
    mostrar_fila_formatada(caixa);

    pthread_mutex_unlock(&caixa->mutex);
}

/* Libera o caixa e aplica o envelhecimento em quem continua esperando*/
void liberar_caixa(Caixa *caixa, Pessoa *pessoa)
{
    pthread_mutex_lock(&caixa->mutex);

    caixa->ocupado = 0;

    printf("%s vai para casa", pessoa->nome);
    mostrar_fila_formatada(caixa);

    if (caixa->tamanho_fila > 0)
    {
        for (int i = 0; i < caixa->tamanho_fila; i++)
        {
            Pessoa *atual = caixa->fila[i];
            if (atual == NULL)
                continue;

            if (!vez_pessoa(caixa, atual))
            {
                atual->frustracoes++;

                if (atual->frustracoes >= 2)
                {
                    atual->frustracoes = 0;
                    if (atual->prioridade_atual < GRAVIDA_OU_CRIANCA_COLO)
                    {
                        atual->prioridade_atual++;
                        printf("Gerente detectou inanição, aumentando prioridade de %s\n", atual->nome);
                    }
                }
            }
        }

        // acorda todo mundo; quem não for a vez volta a dormir no while de esperar_caixa
        for (int i = 0; i < caixa->tamanho_fila; i++)
        {
            if (caixa->fila[i] != NULL)
                pthread_cond_signal(&caixa->cond[caixa->fila[i]->id]);
        }
    }

    pthread_mutex_unlock(&caixa->mutex);
}
