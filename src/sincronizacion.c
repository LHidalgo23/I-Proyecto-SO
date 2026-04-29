#include <stdio.h>
#include <stdlib.h>
#include "sincronizacion.h"
#include "terminal.h"

sem_t          sem_muelles;
pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;

void sincronizacion_init(void) {
    if (sem_init(&sem_muelles, 0, NUM_MUELLES) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

void sincronizacion_destroy(void) {
    sem_destroy(&sem_muelles);
    pthread_mutex_destroy(&mutex_log);
}