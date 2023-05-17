#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
Fase 1 do projeto:
Um grupo de pizzaiolos (de 1 até N pessoas) tem que se
organizar para utilizar os poucos fornos disponíveis na
pizzaria em que trabalham. Os fornos possuem quantidade
de gás variável e se tornam indisponíveis durante o
reabastecimento.
*/

#define N_PIZZAIOLOS 5
#define N_REPOSITORES 1
#define N_FORNOS 2
#define GAS_INICIAL 2 // quantidade máxima de gás

sem_t sem_fornos[N_FORNOS];
int gas_fornos[N_FORNOS];

void* f_pizzaiolo(void* v) {
    int id = *(int*)v;
    int forno_disponivel = -1;

    while (1) {
        // Verifica se há algum forno disponível
        for (int i = 0; i < N_FORNOS; i++) {
            if (sem_trywait(&sem_fornos[i]) == 0) {
                forno_disponivel = i;

                if (gas_fornos[forno_disponivel] == 0) {
                    printf("O forno %d está sem gás.\n", forno_disponivel);
                    sem_post(&sem_fornos[forno_disponivel]);
                    forno_disponivel = -1;
                    continue;
                }

                break;
            }
        }

        if (forno_disponivel != -1) {
            printf("Pizzaiolo %d está usando o forno %d.\n", id, forno_disponivel);
            sleep(1 + random()%2); // Simula o tempo de aquecimento da pizza
            gas_fornos[forno_disponivel]--;
            printf("Pizzaiolo %d terminou de aquecer a pizza no forno %d. Gás restante: %d.\n", id, forno_disponivel, gas_fornos[forno_disponivel]);
            if (gas_fornos[forno_disponivel] == 0) {
                printf("O forno %d ficou sem gás.\n", forno_disponivel);
            }
            sem_post(&sem_fornos[forno_disponivel]);
        } else {
            printf("Pizzaiolo %d está aguardando um forno disponível.\n", id);
            sleep(2); // Simula o tempo de espera
        }

        forno_disponivel = -1; // Reseta o valor do forno disponível
    }

    return NULL;
}

void* f_repositor(void* v) {
    int id = *(int*)v;
    int forno_selecionado = -1;

    while(1) {

        printf("Repositor %d está verificando se há algum forno sem gás.\n", id);
        for (int i = 0; i < N_FORNOS; i++) {
            if (sem_trywait(&sem_fornos[i]) == 0) {
                forno_selecionado = i;

				if (gas_fornos[forno_selecionado] == 0) {
					printf("Repositor %d está reabastecendo o forno %d.\n", id, forno_selecionado);
                    sleep(4); // Simula o tempo de reabastecimento
                    gas_fornos[forno_selecionado] = GAS_INICIAL;
                    printf("Repositor %d terminou de reabastecer o forno %d. Gás restante: %d.\n", id, forno_selecionado, gas_fornos[forno_selecionado]);
					sem_post(&sem_fornos[forno_selecionado]);
					forno_selecionado = -1;
					continue;
				}

                break;
            }
        }

        sleep(random()%7); // Simula o tempo de espera
    }
}

int main() {
    pthread_t thr_pizzaiolos[N_PIZZAIOLOS];
    pthread_t thr_repositores[N_REPOSITORES];

    int i, id_pizzaiolo[N_PIZZAIOLOS], id_repositor[N_REPOSITORES];

    for (i = 0; i < N_FORNOS; i++) {
        sem_init(&sem_fornos[i], 0, 1); // Inicializa os semáforos dos fornos
        gas_fornos[i] = GAS_INICIAL; // Inicializa o gás dos fornos
    }

    for (i = 0; i < N_PIZZAIOLOS; i++) {
        id_pizzaiolo[i] = i;
        pthread_create(&thr_pizzaiolos[i], NULL, f_pizzaiolo, (void*)&id_pizzaiolo[i]);
    }

    for (i = 0; i < N_REPOSITORES; i++) {
        id_repositor[i] = i;
        pthread_create(&thr_repositores[i], NULL, f_repositor, (void*)&id_repositor[i]);
    }

    for (i = 0; i < N_PIZZAIOLOS; i++) {
        pthread_join(thr_pizzaiolos[i], NULL);
    }

    return 0;
}
