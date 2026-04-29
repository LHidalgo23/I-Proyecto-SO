#include "camion.h"
#include "sincronizacion.h"
#include "logger.h"
#include "scheduler.h"
#include <time.h>

extern double tiempo_base_simulacion;
#include <unistd.h>

extern Algoritmo modo_algoritmo;
extern double tiempo_base_simulacion;

static double ahora(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static double ahora_relativo(void) {
    return ahora() - tiempo_base_simulacion;
}

void *hilo_camion(void *arg) {
    Camion *c = (Camion *)arg;


    c->estado = ESTADO_NUEVO;
    log_evento(c->id, "NUEVO", "Camion creado, entrando a cola");


    c->estado = ESTADO_LISTO;
    c->tiempo_llegada = ahora_relativo();
    scheduler_encolar(c);
    log_evento(c->id, "LISTO", "En cola de planificacion");

    Camion *turno;
    do {
        turno = scheduler_siguiente();
        if (turno == NULL || turno->id != c->id) {
            if (turno != NULL) scheduler_encolar(turno); /* devuelve al que no era */
            usleep(5000); /* 5ms */
        }
    } while (turno == NULL || turno->id != c->id);

    c->estado = ESTADO_BLOQUEADO;
    log_evento(c->id, "BLOQUEADO", "Esperando muelle disponible (sem_wait)");
    sem_wait(&sem_muelles);

    /* ── ESTADO: EJECUCIÓN ── */
    c->estado = ESTADO_EJECUCION;
    c->tiempo_inicio = ahora_relativo();
    log_evento(c->id, "EJECUCION", "Ocupando muelle, iniciando carga");

    if (modo_algoritmo == FIFO) {
        sleep(c->burst_restante);
        c->burst_restante = 0;
    } else {
        while (c->burst_restante > 0) {
            int tiempo_turno = (c->burst_restante < QUANTUM)
                               ? c->burst_restante : QUANTUM;
            sleep(tiempo_turno);
            c->burst_restante -= tiempo_turno;

            if (c->burst_restante > 0) {
                sem_post(&sem_muelles);
                log_evento(c->id, "LISTO", "Quantum agotado, re-encolado");
                c->estado = ESTADO_LISTO;
                scheduler_encolar(c);

                Camion *t2;
                do {
                    t2 = scheduler_siguiente();
                    if (t2 == NULL || t2->id != c->id) {
                        if (t2 != NULL) scheduler_encolar(t2);
                        usleep(5000);
                    }
                } while (t2 == NULL || t2->id != c->id);

                c->estado = ESTADO_BLOQUEADO;
                log_evento(c->id, "BLOQUEADO", "Esperando muelle disponible (sem_wait)");
                sem_wait(&sem_muelles);
                c->estado = ESTADO_EJECUCION;
                log_evento(c->id, "EJECUCION", "Retoma muelle tras quantum");
            }
        }
    }

    /* Libera el muelle */
    sem_post(&sem_muelles);
    c->tiempo_fin = ahora_relativo();

    /* TERMINADO */
    c->estado = ESTADO_TERMINADO;
    log_evento(c->id, "TERMINADO", "Carga completada, muelle liberado");

    return NULL;
}
