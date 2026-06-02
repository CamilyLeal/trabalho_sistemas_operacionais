#ifndef PESSOA_H
#define PESSOA_H

typedef enum {
    COMUM = 0,
    DEFICIENTE = 1,
    IDOSO = 2,
    GRAVIDA_OU_CRIANÇA_COLO = 3
} Prioridade;

typedef struct {
    int id;
    char nome[20];

    Prioridade prioridade_inicial;
    Prioridade prioridade_atual;

    int frustracoes;
    int ordem_chegada;
} Pessoa;

Pessoa criar_pessoa(int id, char *nome, Prioridade prioridade);
void *rotina_pessoa(void *arg);
#endif