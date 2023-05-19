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

#define N_PIZZAIOLOS 20
#define N_REPOSITORES 2
#define N_FORNOS 6
#define GAS_INICIAL 8 // quantidade máxima de gás

sem_t sem_fornos[N_FORNOS];
int gas_fornos[N_FORNOS];

int forno_usado_por[N_FORNOS]; // para fins de print com ncurses
int forno_abastecido_por[N_FORNOS];

sem_t sem_le_abastecimento; // evita que o vetor seja acessado por multiplas thread ao mesmo tempo
sem_t sem_escreve_abastecimento; // evita que o vetor seja acessado por multiplas thread ao mesmo tempo

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
            sleep(2); // evita que o pizzaiolo use o mesmo forno vezes seguidas
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

        sem_wait(&sem_escreve_abastecimento);
        //printf("Repositor %d está verificando se há algum forno Sem gás.\n", id);
        for (int i = 0; i < N_FORNOS; i++) {
            forno_abastecido_por[i] = -1;
            if (sem_trywait(&sem_fornos[i]) == 0) {
                forno_selecionado = i;
                
				if (gas_fornos[forno_selecionado] == 0) {
                    forno_abastecido_por[i] = id;
					//printf("Repositor %d está reabastecendo o forno %d.\n", id, forno_selecionado);
                    sleep(3); // Simula o tempo de reabastecimento
                    gas_fornos[forno_selecionado] = GAS_INICIAL;
                    //printf("Repositor %d terminou de reabastecer o forno %d. Gás restante: %d.\n", id, forno_selecionado, gas_fornos[forno_selecionado]);
				}
                sem_post(&sem_fornos[forno_selecionado]);
                forno_abastecido_por[i] = -1;
            }
        }
        sem_post(&sem_le_abastecimento);
    }
}

void printLogo() {
    char logo_pizza[6][24] = {
        "  _____ _              \0",
        " |  __ (_)             \0",
        " | |__) ) __________ _ \0",
        " |  ___| |_  |_  / _` |\0",
        " | |   | |/ / / | (_| |\0",
        " |_|   |_/___/___\\__,_\0"
    };
    char logo_504[6][21] = {
        " _____  ___  _  _   \0",
        "| ____|/ _ \\| || |  \0",
        "| |__ | | | | || |_ \0",
        "|___ \\| | | |__   _|\0",
        " ___) | |_| |  | |  \0",
        "|_____/\\___/   |_|  \0"
    };

    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("Seu terminal não suporta cores.\n");
        return 1;
    }
    start_color(); // Inicia as cores
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    for (int i = 0; i < 6; i++) {
        attron(COLOR_PAIR(1));
        mvprintw(i, 0, logo_pizza[i]);
        attron(COLOR_PAIR(2));
        mvprintw(i, 22, logo_504[i]);
    }
    // attroff(COLOR_PAIR(1));
    attroff(COLOR_PAIR(2));
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
                mvprintw(15+j, 14*i, forno_parado[j]);
            }
        } else {
            for (int j=0; j<6; j++) {
                mvprintw(15+j, 14*i, forno_assando[j]);
                mvprintw(24+j, 14*i, pizzaiolo[j]);
            }
            mvprintw(28, 14*i, "pizzai.: %d\0", forno_usado_por[i]);
        }

        char str[3];
        mvprintw(22, 14*i, "gás: %d\0", gas_fornos[i]);
    }
}

void updateRepositores() {
    char repositor[6][11] = {
        " ___   0  \0",
        "[ G ]_/|\\ \0",
        "       |  \0",
        "      / \\ \0",
        "     /   \\\0",
        "__________\0"
    };

    sem_wait(&sem_le_abastecimento);
    for (int i=0; i<N_FORNOS; i++) {
        if(forno_abastecido_por[i] != -1) {
            for (int j = 0; j < 6; j++)
            {
                mvprintw(8+j, 14*i, repositor[j]);
            }
            mvprintw(14, 14*i, "reposit.: %d\0", forno_abastecido_por[i]);
        }
    }
    sem_post(&sem_escreve_abastecimento);
}

void* f_animate(void* v) {
    initscr();

    while(1) {
        usleep(750000);
        clear();
        printLogo();
        updateRepositores();
        updateFornos();
        refresh();
    }
}

int main() {
    pthread_t thr_pizzaiolos[N_PIZZAIOLOS];
    pthread_t thr_repositores[N_REPOSITORES];
    pthread_t thr_animate;

    int i, id_pizzaiolo[N_PIZZAIOLOS], id_repositor[N_REPOSITORES];

    sem_init(&sem_le_abastecimento, 0, 1);
    sem_init(&sem_escreve_abastecimento, 0, 1);

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
