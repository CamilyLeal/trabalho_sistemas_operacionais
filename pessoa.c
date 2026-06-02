#include <string.h>
#include <stdio.h>
#include "pessoa.h"

Pessoa criar_pessoa(int id, char *nome, Prioridade prioridade)
{
    Pessoa pessoa;

    pessoa.id = id;

    strcpy(pessoa.nome, nome);

    pessoa.prioridade_inicial = prioridade;
    pessoa.prioridade_atual = prioridade;

    pessoa.frustracoes = 0;
    pessoa.ordem_chegada = -1;

    return pessoa;
}


void *rotina_pessoa(void *arg)
{
    Pessoa *p = (Pessoa *) arg;

    printf("%s iniciou\n", p->nome);

    return NULL;
}