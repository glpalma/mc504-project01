#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/*
Fase 1 do projeto:
Um grupo de pizzaiolos (de 1 até N pessoas) tem que se
organizar para utilizar os poucos fornos disponíveis na
pizzaria em que trabalham. Os fornos possuem quantidade
de gás variável e se tornam indisponíveis durante o
reabastecimento.
*/

#define N_PIZZAIOLOS 5
#define N_FORNOS 2

#define GAS_INICIAL 8 // quantidade máxima de gás

sem_t sem_escreve_indicador, sem_le_indicador;
int indicador; // indica aos pizzaiolos qual forno utilizar

typedef enum { PARADO, REABASTECENDO_GAS, ASSANDO } estado_forno;
estado_forno e_forno[N_FORNOS];
int qtd_gas_fornos[N_FORNOS];

sem_t sem_estados;

/**
 * @brief Função responsável por representar o funcionamento de um forno.
 *
 * @param id_forno
 * @return void*
 */
void* f_forno(void* id_forno) {
    int id = *(int*)id_forno;

    // forno demora a reabastecer
    if (qtd_gas_fornos[id] < GAS_INICIAL) {
        qtd_gas_fornos[id] += 1;
        return;
    }

    // indica aos pizzaiolos qual forno utilizar
    sem_wait(&sem_escreve_indicador);
    indicador = id;
    sem_post(&sem_le_indicador);

    sem_wait(&sem_estados);
    e_forno[id] = ASSANDO;
    sem_post(&sem_estados);
    sleep(random() % 3);
    // precisa de lock aqui sendo que um forno n acessa o gás do outro?
    qtd_gas_fornos[id] -= 1;

    sem_wait(&sem_estados);
    e_forno[id] = PARADO;
    sem_post(&sem_estados);

    if (qtd_gas_fornos[id] == 0) {
        sem_wait(&sem_estados);
        e_forno[id] = REABASTECENDO_GAS;
        sem_post(&sem_estados);
    }
}

/**
 * @brief Representa as ações de um pizzaiolo.
 *
 * @param id_pizzaiolo
 * @return void*
 */
void* f_pizzaiolo(void* id_pizzaiolo) { // TODO: implementar
    int id_pizzaiolo = *(int*)id_pizzaiolo;

    // é assumido que os pizzaiolos sempre estão trabalhando
    while (1) {

    }


}

int main() {
    pthread_t thr_pizzaiolos[N_PIZZAIOLOS], thr_fornos[N_FORNOS];
    int i, id_forno[N_FORNOS], id_pizzaiolo[N_PIZZAIOLOS];

    sem_init(&qtd_gas_fornos, 0, 1);
    sem_init(&sem_estados, 0, 1);

    for (i = 0; i < N_FORNOS; i++) {
        qtd_gas_fornos[N_FORNOS] = GAS_INICIAL;
    }

    for (i = 0; i < N_FORNOS; i++) {
        id_forno[i] = i;
        pthread_create(&thr_fornos[i], NULL, f_forno, (void*)&id_forno[i]);
    }

    for (i = 0; i < N_PIZZAIOLOS; i++) {
        id_forno[i] = i;
        pthread_create(&thr_pizzaiolos[i], NULL, f_forno, (void*)&id_forno[i]);
    }

    for (i = 0; i < N_FORNOS; i++) {
        pthread_join(thr_fornos[i], NULL);
    }

    return 0;
}
