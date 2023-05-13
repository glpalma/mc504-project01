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
int indicador; // Indica aos pizzaiolos qual forno utilizar

typedef enum { PARADO, REABASTECENDO_GAS, ASSANDO } estado_forno;
estado_forno e_forno[N_FORNOS];
int qtd_gas_fornos[N_FORNOS];

typedef enum { MONTANDO_PIZZA, LEVANDO_PIZZA_FORNO, ESPERANDO_FORNO, ENTREGANDO_PIZZA } estado_pizzaiolo;
estado_pizzaiolo e_pizzaiolo[N_PIZZAIOLOS];

sem_t sem_estados;

/**
 * @brief Função responsável por representar o funcionamento de um forno.
 *
 * @param id_forno
 * @return void*
 */
void* f_forno(void* id_forno) {
    int id = *(int*)id_forno;
    sem_wait(&sem_estados);
    estado_forno estado_atual = e_forno[id];
    sem_post(&sem_estados);

    // forno demora a reabastecer
    if (estado_atual == REABASTECENDO_GAS && qtd_gas_fornos[id] < GAS_INICIAL) {
        qtd_gas_fornos[id] += 1;
        sleep(1);
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
 * @brief Devolve um sabor de pizza aleatório do menu.
 *
 * @return sabor de pizza representado por um char
 */
char gerar_sabor_pizza() {
    const int n_itens_menu = 3;
    char menu[n_itens_menu] = { 'Q', 'P', 'C' };

    int selecionado = random() % n_itens_menu;

    return menu[selecionado];
}

/**
 * @brief Representa as ações de um pizzaiolo.
 *
 * @param id_pizzaiolo
 * @return void*
 */
void* f_pizzaiolo(void* id_pizzaiolo) {
    int id = *(int*)id_pizzaiolo;
    char sabor_pizza;
    int meu_forno;

    // os pizzaiolos sempre estão trabalhando
    while (1) {
        // estados do pizzaiolo: montando_pizza, levando_ao_forno, esperando_forno, entregando 

        // pizzaiolo pega um pedido aleatório (fazer gerador de pedidos?)
        // demora um tempo montando a pizza (sleep)
        // verifica qual forno utilizar (acessa o indicador)
        // vai até o forno (somente na animação) (estado de "andando?")
        // espera até a pizza ficar pronta
        // -- precisa existir uma comunicação entre o forno e o pizzaiolo
        // -- para indicar que a pizza ficou pronta
        // pizzaiolo retira do forno e manda servir
        // recomeça

        sabor_pizza = gerar_sabor_pizza();
        sem_wait(&sem_estados);
        e_pizzaiolo[id] = MONTANDO_PIZZA;
        sem_post(&sem_estados);
        sleep(1);

        sem_post(&sem_le_indicador);
        meu_forno = indicador;
        sem_wait(&sem_escreve_indicador);
        
        sem_wait(&sem_estados);
        e_pizzaiolo[id] = LEVANDO_PIZZA_FORNO;
        sem_post(&sem_estados);
        // como representar o movimento do pizzaiolo até o forno?

        // -> semaforo para indicar ao forno a presença de pizza

        sem_wait(&sem_estados);
        e_pizzaiolo[id] = ESPERANDO_FORNO;
        sem_post(&sem_estados);

        // -> forno indica ao pizzaiolo que a pizza está pronta

        sem_wait(&sem_estados);
        e_pizzaiolo[id] = ENTREGANDO_PIZZA;
        sem_post(&sem_estados);

        // como representar o a entrega da pizza ao garçom?
    }
}

int main() {
    pthread_t thr_pizzaiolos[N_PIZZAIOLOS], thr_fornos[N_FORNOS];
    int i, id_forno[N_FORNOS], id_pizzaiolo[N_PIZZAIOLOS];

    sem_init(&qtd_gas_fornos, 0, 1);
    sem_init(&sem_estados, 0, 1);

    for (i = 0; i < N_FORNOS; i++) {
        qtd_gas_fornos[N_FORNOS] = GAS_INICIAL;

        sem_wait(&sem_estados);
        e_forno[i] = PARADO;
        sem_post(&sem_estados);

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
