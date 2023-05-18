#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

/*
Fase 1 do projeto:
Um grupo de pizzaiolos (de 1 até N pessoas) tem que se
organizar para utilizar os poucos fornos disponíveis na
pizzaria em que trabalham. Os fornos possuem quantidade
de gás variável e se tornam indisponíveis durante o
reabastecimento. Os repositores verificam
periodicamente os fornos para reabastecê-los caso
necessário.
*/

#define N_PIZZAIOLOS 7
#define N_REPOSITORES 2
#define N_FORNOS 6
#define GAS_INICIAL 2 // quantidade máxima de gás

sem_t sem_fornos[N_FORNOS];
int gas_fornos[N_FORNOS];
int forno_usado_por[N_FORNOS]; // para fins de print com ncurses

void* f_pizzaiolo(void* v) {
    int id = *(int*)v;
    int forno_disponivel = -1;

    while (1) {
        // Verifica se há algum forno disponível
        for (int i = 0; i < N_FORNOS; i++) {
            if (sem_trywait(&sem_fornos[i]) == 0) {
                forno_disponivel = i;
                if (gas_fornos[forno_disponivel] == 0) {
                    //printf("O forno %d está sem gás.\n", forno_disponivel);
                    sem_post(&sem_fornos[forno_disponivel]);
                    forno_disponivel = -1;
                    continue;
                }
                forno_usado_por[forno_disponivel] = id;
                break;
            }
        }

        if (forno_disponivel != -1) {
            //printf("Pizzaiolo %d está usando o forno %d.\n", id, forno_disponivel);
            sleep(1 + random()%2); // Simula o tempo de aquecimento da pizza
            gas_fornos[forno_disponivel]--;
            //printf("Pizzaiolo %d terminou de aquecer a pizza no forno %d. Gás restante: %d.\n", id, forno_disponivel, gas_fornos[forno_disponivel]);
            if (gas_fornos[forno_disponivel] == 0) {
                //printf("O forno %d ficou sem gás.\n", forno_disponivel);
            }
            sem_post(&sem_fornos[forno_disponivel]);
            forno_usado_por[forno_disponivel] = -1;
        } else {
            //printf("Pizzaiolo %d está aguardando um forno disponível.\n", id);
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

        //printf("Repositor %d está verificando se há algum forno sem gás.\n", id);
        for (int i = 0; i < N_FORNOS; i++) {
            if (sem_trywait(&sem_fornos[i]) == 0) {
                forno_selecionado = i;

				if (gas_fornos[forno_selecionado] == 0) {
					//printf("Repositor %d está reabastecendo o forno %d.\n", id, forno_selecionado);
                    sleep(4); // Simula o tempo de reabastecimento
                    gas_fornos[forno_selecionado] = GAS_INICIAL;
                    //printf("Repositor %d terminou de reabastecer o forno %d. Gás restante: %d.\n", id, forno_selecionado, gas_fornos[forno_selecionado]);
				}
                sem_post(&sem_fornos[forno_selecionado]);
            }
        }

        sleep(random()%5); // Simula o tempo de espera
    }
}

void printLogo() {
    char logo[6][43] = {
        "  _____ _              _____  ___  _  _   \0",
        " |  __ (_)            | ____|/ _ \\| || |  \0",
        " | |__) _ __________ _| |__ | | | | || |_ \0",
        " |  ___| |_  |_  / _` |___ \\| | | |__   _|\0",
        " | |   | |/ / / | (_| |___) | |_| |  | |  \0",
        " |_|   |_/___/___\\__,_|____/ \\___/   |_|  \0"
    };

    for (int i = 0; i < 6; i++) {
        mvprintw(i, 0, logo[i]);
    }
}


void updateFornos() {

    char pizzaiolo[6][11] = {
        "     0    \0",
        "   _/|\\   \0",
        "     |    \0",
        "    / \\   \0",
        "   /   \\  \0",
        "__________\0"
    };

    char forno_parado[6][11] = {
        "__________\0",
        "| ` ` ` `|\0",
        "|  ____  |\0",
        "| |    | |\0",
        "| |____| |\0",
        "|________|\0"
    };

    char forno_assando[6][11] = {
        "__________\0",
        "| `*`*`*`|\0",
        "| *____* |\0",
        "| |/\\/\\| |\0",
        "| |____| |\0",
        "|________|\0"
    };

    for (int i=0; i<N_FORNOS; i++) {
        if (forno_usado_por[i] == -1) {
            for (int j=0; j<6; j++) {
                mvprintw(8+j, 11*i, forno_parado[j]);
            }
        } else {
            for (int j=0; j<6; j++) {
                mvprintw(8+j, 11*i, forno_assando[j]);
                mvprintw(17+j, 11*i, pizzaiolo[j]);
            }
            mvprintw(21, 11*i, "pizzai.: %d\0", forno_usado_por[i]);
        }

        char str[3];
        mvprintw(15, 11*i, "gas: %d\0", gas_fornos[i]);
    }
}

void updatePizzaiolos() {

}

void updateRepositores() {

}

void* f_animate(void* v) {
    initscr();

    while(1) {
        usleep(750000);
        clear();
        printLogo();
        updateFornos();
        updatePizzaiolos();
        updateRepositores();
        refresh();
    }
}

int main() {
    pthread_t thr_pizzaiolos[N_PIZZAIOLOS];
    pthread_t thr_repositores[N_REPOSITORES];
    pthread_t thr_animate;

    int i, id_pizzaiolo[N_PIZZAIOLOS], id_repositor[N_REPOSITORES];

    for (i = 0; i < N_FORNOS; i++) {
        forno_usado_por[i] = -1; // Inicializa os fornos como disponíveis
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

    pthread_create(&thr_animate, NULL, f_animate, NULL);

    for (i = 0; i < N_PIZZAIOLOS; i++) {
        pthread_join(thr_pizzaiolos[i], NULL);
    }

    return 0;
}
