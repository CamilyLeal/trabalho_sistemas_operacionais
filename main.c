#include <stdio.h>
#include "pessoa.h"
#include "caixa.h"
#include <pthread.h>

int main()
{
    Caixa caixa;

    iniciar_caixa(&caixa);

    Pessoa pessoas[MAX_PESSOAS] = {
        criar_pessoa(0, "Maria", GRAVIDA_OU_CRIANÇA_COLO),
        criar_pessoa(1, "Marcos", GRAVIDA_OU_CRIANÇA_COLO),
        criar_pessoa(2, "Vanda", IDOSO),
        criar_pessoa(3, "Valter", IDOSO),
        criar_pessoa(4, "Paula", DEFICIENTE),
        criar_pessoa(5, "Pedro", DEFICIENTE),
        criar_pessoa(6, "Sueli", COMUM),
        criar_pessoa(7, "Silas", COMUM)};

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

    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pthread_join(threads[i], NULL); //espera a thread atual terminar
    }

    destruir_caixa(&caixa);

    return 0;
}