#include "caixa.h"

void iniciar_caixa(Caixa *caixa)
{
    caixa->ocupado = 0;
    caixa->tamanho_fila = 0;
    caixa->ordem = 0;\

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