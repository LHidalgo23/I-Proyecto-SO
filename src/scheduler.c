#include "scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_COLA 64

static Camion         *cola[MAX_COLA];
static int             cabeza = 0, cola_fin = 0, tam = 0;
static Algoritmo       algoritmo_actual;
static pthread_mutex_t mutex_cola = PTHREAD_MUTEX_INITIALIZER;

void scheduler_init(Algoritmo alg) {
    algoritmo_actual = alg;
    cabeza = cola_fin = tam = 0;
}

void scheduler_encolar(Camion *c) {
    pthread_mutex_lock(&mutex_cola);
    if (tam < MAX_COLA) {
        cola[cola_fin] = c;
        cola_fin = (cola_fin + 1) % MAX_COLA;
        tam++;

        if (algoritmo_actual == ROUND_ROBIN) {
            int pos  = (cola_fin - 1 + MAX_COLA) % MAX_COLA;
            int prev = (pos - 1 + MAX_COLA) % MAX_COLA;
            int i = tam;
            while (i > 1 && cola[prev]->prioridad > cola[pos]->prioridad) {
                Camion *tmp = cola[prev];
                cola[prev]  = cola[pos];
                cola[pos]   = tmp;
                pos  = prev;
                prev = (pos - 1 + MAX_COLA) % MAX_COLA;
                i--;
            }
        }
    }
    pthread_mutex_unlock(&mutex_cola);
}

Camion *scheduler_siguiente(void) {
    pthread_mutex_lock(&mutex_cola);
    if (tam == 0) {
        pthread_mutex_unlock(&mutex_cola);
        return NULL;
    }
    Camion *c = cola[cabeza];
    cabeza = (cabeza + 1) % MAX_COLA;
    tam--;
    pthread_mutex_unlock(&mutex_cola);
    return c;
}

int scheduler_vacia(void) {
    return tam == 0;
}

void scheduler_destroy(void) {
    pthread_mutex_destroy(&mutex_cola);
}
