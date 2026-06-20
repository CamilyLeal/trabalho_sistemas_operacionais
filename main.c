#include <stdio.h>
#include "pessoa.h"
#include "caixa.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "gerente.h"

int main( int argc, char *argv[])
{
    if (argc < 2)//valida se os argumentos foram passados corretamente
    {
        printf("Erro: Você deve passar o número de tentativas.\n");
        return 1; // Encerra o programa com erro
    }

    // Converte o argumento para um número inteiro
    int tentativas = atoi(argv[1]);

    if (tentativas <= 0)
    {
        printf("Erro: O número de tentativas deve ser maior que 0.\n");
        return 1;
    }

    srand(time(NULL));// Inicializa a semente para geração de números aleatórios

    Caixa caixa;

    iniciar_caixa(&caixa);

    Pessoa pessoas[MAX_PESSOAS] = {
        criar_pessoa(0, "Maria", GRAVIDA_OU_CRIANCA_COLO),
        criar_pessoa(1, "Marcos", GRAVIDA_OU_CRIANCA_COLO),
        criar_pessoa(2, "Vanda", IDOSO),
        criar_pessoa(3, "Valter", IDOSO),
        criar_pessoa(4, "Paula", DEFICIENTE),
        criar_pessoa(5, "Pedro", DEFICIENTE),
        criar_pessoa(6, "Sueli", COMUM),
        criar_pessoa(7, "Silas", COMUM)};

    // Atribui o caixa compartilhado e o número de tentativas para cada pessoa    
    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pessoas[i].caixa_compartilhado = &caixa;
        pessoas[i].total_tentativas = tentativas;
    }

    printf(" ==== BEM VINDO A FORMIGÓPOLIS ==== \n");

    pthread_t threads[MAX_PESSOAS];  //armazena os identificadores das threads
    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_create( //cria a thread de cada pessoa
            &threads[i],
            NULL,
            rotina_pessoa,
            &pessoas[i]);
    }
    
    pthread_t thread_gerente;
    pthread_create(&thread_gerente, NULL, rotina_gerente, &caixa);

    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_join(threads[i], NULL); //espera a thread atual terminar
    }

    pthread_cancel(thread_gerente);
    pthread_join(thread_gerente, NULL); // Garante que o gerente parou mesmo
    destruir_caixa(&caixa);
    
    return 0;
}