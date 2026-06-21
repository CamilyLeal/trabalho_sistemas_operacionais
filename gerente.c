#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gerente.h"
#include "caixa.h"

 /* Deadlock ocorre quando o caixa está livre mas tem pelo menos um representante de cada categoria prioritária esperando
 Escolhe uma dessas pessoas aleatoriamente e força o atendimento. */
void *rotina_gerente(void *arg)
{
    Caixa *caixa = (Caixa *)arg;

    printf("O gerente começou a trabalhar e está fiscalizando a lotérica.\n");

    while (1)
    {
        sleep(5);

        pthread_mutex_lock(&caixa->mutex);

        if (caixa->ocupado == 0 && caixa->tamanho_fila >= 3)
        {
            int tem_gravida = 0, tem_idoso = 0, tem_deficiente = 0;
            int id_gravida = -1, id_idoso = -1, id_deficiente = -1;

            for (int i = 0; i < caixa->tamanho_fila; i++)
            {
                if (caixa->fila[i]->prioridade_atual == GRAVIDA_OU_CRIANCA_COLO)
                {
                    tem_gravida = 1;
                    id_gravida = caixa->fila[i]->id;
                }
                if (caixa->fila[i]->prioridade_atual == IDOSO)
                {
                    tem_idoso = 1;
                    id_idoso = caixa->fila[i]->id;
                }
                if (caixa->fila[i]->prioridade_atual == DEFICIENTE)
                {
                    tem_deficiente = 1;
                    id_deficiente = caixa->fila[i]->id;
                }
            }

            // deadlock confirmado: uma pessoa de cada categoria presa no ciclo
            if (tem_gravida && tem_idoso && tem_deficiente)
            {
                int ids[3] = {id_gravida, id_idoso, id_deficiente};
                int sorteado = ids[rand() % 3];

                char *nome_liberado = "";
                Pessoa *vitima = NULL;
                for (int i = 0; i < caixa->tamanho_fila; i++)
                {
                    if (caixa->fila[i]->id == sorteado)
                    {
                        vitima = caixa->fila[i];
                        nome_liberado = caixa->fila[i]->nome;
                        break;
                    }
                }

                printf("Gerente detectou deadlock, liberando %s para atendimento\n", nome_liberado);

                // vitima_forcada faz vez_pessoa() retornar 1 só pra essa pessoa
                caixa->vitima_forcada = vitima;
                pthread_cond_signal(&caixa->cond[sorteado]);
            }
        }

        pthread_mutex_unlock(&caixa->mutex);
    }

    return NULL;
}
