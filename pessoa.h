#ifndef PESSOA_H
#define PESSOA_H

typedef enum {
    COMUM = 0,
    DEFICIENTE = 1,
    IDOSO = 2,
    GRAVIDA_OU_CRIANCA_COLO = 3
} Prioridade;

typedef struct {
    int id;
    char nome[20];

    Prioridade prioridade_inicial; // prioridade real da pessoa (nunca muda)
    Prioridade prioridade_atual;   // pode subir por inanição, volta ao inicial após ser atendida

    int frustracoes;    // quantas vezes seguidas foi preterida na fila
    int ordem_chegada;  // usado pra desempatar pessoas de mesma prioridade

    void *caixa_compartilhado;
    int total_tentativas;
} Pessoa;

Pessoa criar_pessoa(int id, char *nome, Prioridade prioridade);
void *rotina_pessoa(void *arg);

#endif
