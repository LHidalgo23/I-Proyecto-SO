#ifndef SINCRONIZACION_H
#define SINCRONIZACION_H

#include <semaphore.h>
#include <pthread.h>

extern sem_t            sem_muelles;
extern pthread_mutex_t  mutex_log;

void sincronizacion_init(void);
void sincronizacion_destroy(void);

#endif /* SINCRONIZACION_H */
