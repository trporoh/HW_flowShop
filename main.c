#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

unsigned int shops[5];
int clients[3];
int need[3];
unsigned int loader;

pthread_mutex_t mutex[5];

int Checker() {
    for (unsigned int i = 0; i < 3; i++) {
        if (clients[i] > 0) {
            return 1;
        }
    }
    return 0;
}

void Loader() {
    printf("loader start!\n");
    while (Checker()) {
        for (unsigned int i = 0; i < 5; i++) {
            if (pthread_mutex_trylock(&mutex[i])) {
                printf("mutex lock failed in thread %ld (loader), mutex %d isn't available!\n", pthread_self(), i);
                continue;
            }
            shops[i] += loader;
            printf("loader sleep in mutex %d, shop has %d!\n", i, shops[i]);
            sleep(1);
            pthread_mutex_unlock(&mutex[i]);
        }
    }
    printf("Loader finished!\n");
}

void Clients(unsigned int* ptr_id) {

    unsigned int id = *(ptr_id);
    need[id] = rand() % 1000 + 9500;

    printf("thread %d start!\n", id);
 

    while (clients[id] < need[id]) {
        for (unsigned int i = 0; i < 5; i++) {
            if (pthread_mutex_trylock(&mutex[i])) {
                printf("mutex lock failed in thread %ld (client), mutex %d isn't available, client id %d!\n", pthread_self(), i, id);
                continue;
            }
            clients[id] += shops[i];
            shops[i] = 0;
            printf("thread %ld, client id %d, client need %d, sleep in mutex %d!\n", pthread_self(), id, need[id] - clients[id], i);
            sleep(2);
            if (pthread_mutex_unlock(&mutex[i])) {
                printf(" error mutex unlock failed in thread % ld(client), number of mutex % d, client id % d!\n", pthread_self(), i, id);
                return;
            }
        }
    }
    printf("thread %d finished, client need was %d!\n", id, clients[id]);
}

void Init() {
    for (unsigned int i = 0; i < 5; i++) {
        if (pthread_mutex_init(&mutex[i], NULL)) {
            printf("mutex init error\n");
            exit(EXIT_FAILURE);
        }
        shops[i] = rand() % 600 + 500;
    }

    for (int i = 0; i < 3; i++) {
        clients[i] = 0;
    }

    loader = 500;
}

void FinnalInfo() {
    printf("shops - %d, %d, %d, %d, %d\n", shops[0], shops[1], shops[2], shops[3], shops[4]);
    printf("clients - %d, %d, %d need - %d, %d, %d\n", clients[0], clients[1], clients[2], need[0], need[1], need[2]);
}

int main() {

    Init();
    pthread_t tid[4];
    unsigned int id[3] = { 0, 1, 2 };

    for (unsigned int i = 0; i < 3; i++) {
        pthread_create(&tid[i], NULL, Clients, (void*)&id[i]);
    }

    pthread_create(&tid[3], NULL, Loader, NULL);

    for (unsigned int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    FinnalInfo();

    exit(EXIT_SUCCESS);
}