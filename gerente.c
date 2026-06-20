#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gerente.h"
#include "caixa.h"

void *rotina_gerente(void *arg)
{
    Caixa *caixa = (Caixa *) arg;
    printf("O gerente começou a trabalhar e está fiscalizando a lotérica.\n");

    while(1)
    {
        sleep(5); // O gerente verifica a cada 5 segundos

        pthread_mutex_lock(&caixa->mutex);

        if (caixa->ocupado == 1 && caixa->tamanho_fila > 1)
        {
            Pessoa *proximo = NULL;

            for (int i = 0; i < caixa->tamanho_fila; i++)
            {
                if (caixa->fila[i] != NULL)
                {
                    if (proximo == NULL)
                    {
                        proximo = caixa->fila[i];
                    }
                    else if (caixa->fila[i]->prioridade_atual > proximo->prioridade_atual)
                    {
                        proximo = caixa->fila[i];
                    }
                    else if (caixa->fila[i]->prioridade_atual == proximo->prioridade_atual &&
                             caixa->fila[i]->ordem_chegada < proximo->ordem_chegada)
                    {
                        proximo = caixa->fila[i];
                    }
                }
            }

            for (int i = 0; i < caixa->tamanho_fila; i++)
            {
                if (caixa->fila[i] != NULL && caixa->fila[i] != proximo)
                {
                    if (caixa->fila[i]->ordem_chegada < proximo->ordem_chegada)
                    {
                        caixa->fila[i]->frustracoes++;

                        if (caixa->fila[i]->frustracoes >= 2 && caixa->fila[i]->prioridade_atual < GRAVIDA_OU_CRIANCA_COLO)
                        {
                            caixa->fila[i]->prioridade_atual = GRAVIDA_OU_CRIANCA_COLO;
                            printf("Gerente detectou inanição, aumentando prioridade de %s\n", caixa->fila[i]->nome);
                        }
                    }
                }
            }
        }

        if (caixa->ocupado == 0 && caixa->tamanho_fila >= 3)
        {
            int tem_gravida = 0;
            int tem_idoso = 0;
            int tem_deficiente = 0;

            int id_gravida = -1;
            int id_idoso = -1;
            int id_deficiente = -1;

            for (int i = 0; i < caixa->tamanho_fila; i++)
            {
                if (caixa->fila[i]->prioridade_inicial == GRAVIDA_OU_CRIANCA_COLO)
                {
                    tem_gravida = 1;
                    id_gravida = caixa->fila[i]->id;
                }
                if (caixa->fila[i]->prioridade_inicial == IDOSO)
                {
                    tem_idoso = 1;
                    id_idoso = caixa->fila[i]->id;
                }
                if (caixa->fila[i]->prioridade_inicial == DEFICIENTE)
                {
                    tem_deficiente = 1;
                    id_deficiente = caixa->fila[i]->id;
                }
            }

            if (tem_gravida && tem_idoso && tem_deficiente)
            {
                int IDs[3] = {id_gravida, id_idoso, id_deficiente};
                int sorteado = IDs[rand() % 3];

                char *nome_liberado = "";
                for (int i = 0; i < caixa->tamanho_fila; i++)
                {
                    if (caixa->fila[i]->id == sorteado) { nome_liberado = caixa->fila[i]->nome; break; }
                }

                printf("\nGerente detectou deadlock, liberando %s para atendimento\n", nome_liberado);
                pthread_cond_signal(&caixa->cond[sorteado]);
            }
        }
        else if (caixa->ocupado == 0 && caixa->tamanho_fila > 0)
        {
            if (caixa->fila[0] != NULL)
            {
                pthread_cond_signal(&caixa->cond[caixa->fila[0]->id]);
            }
        }

        pthread_mutex_unlock(&caixa->mutex);
    }
    return NULL;
}