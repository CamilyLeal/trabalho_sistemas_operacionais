#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pessoa.h"
#include "caixa.h"

Pessoa criar_pessoa(int id, char *nome, Prioridade prioridade)
{
    Pessoa pessoa;

    pessoa.id = id;
    strcpy(pessoa.nome, nome);

    pessoa.prioridade_inicial = prioridade;
    pessoa.prioridade_atual = prioridade;

    pessoa.frustracoes = 0;
    pessoa.ordem_chegada = -1;

    pessoa.caixa_compartilhado = NULL;
    pessoa.total_tentativas = 0;

    return pessoa;
}

/* Rotina de cada thread de pessoa. Repete o ciclo 'total_tentativas' vezes:
 * faz outras coisas por 3-5 segundos, entra na fila, é atendida por 1 segundo, vai embora. */
void *rotina_pessoa(void *arg)
{
    Pessoa *p = (Pessoa *) arg;

    for (int i = 0; i < p->total_tentativas; i++)
    {
        int tempo_outras_coisas = 3 + (rand() % 3);
        sleep(tempo_outras_coisas);

        esperar_caixa((Caixa *)p->caixa_compartilhado, p);

        sleep(1); // tempo de atendimento no caixa

        liberar_caixa((Caixa *)p->caixa_compartilhado, p);
    }

    return NULL;
}
