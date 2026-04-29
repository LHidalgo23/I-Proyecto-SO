#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "terminal.h"
#include "sincronizacion.h"
#include "logger.h"
#include "scheduler.h"
#include "camion.h"

Algoritmo modo_algoritmo;
double tiempo_base_simulacion;

/* Datos fijos de los camiones: {id, prioridad, burst}
   Prioridad 1 = perecedero (alta), 2 = normal */
static int datos[NUM_CAMIONES][3] = {
    {1, 2, 5},
    {2, 1, 3},
    {3, 2, 4},
    {4, 1, 2},
    {5, 2, 6},
    {6, 1, 1},
    {7, 2, 3},
    {8, 1, 4},
};

static void ejecutar_simulacion(Algoritmo alg, Camion camiones[], pthread_t tids[]) {
    for (int i = 0; i < NUM_CAMIONES; i++) {
        camiones[i].id             = datos[i][0];
        camiones[i].prioridad      = datos[i][1];
        camiones[i].burst          = datos[i][2];
        camiones[i].burst_restante = datos[i][2];
        camiones[i].estado         = ESTADO_NUEVO;
        camiones[i].tiempo_llegada = 0;
        camiones[i].tiempo_inicio  = 0;
        camiones[i].tiempo_fin     = 0;
    }

    scheduler_init(alg);
    tiempo_base_simulacion = ({
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_sec + ts.tv_nsec / 1e9;
    });

    for (int i = 0; i < NUM_CAMIONES; i++)
        pthread_create(&tids[i], NULL, hilo_camion, &camiones[i]);

    for (int i = 0; i < NUM_CAMIONES; i++)
        pthread_join(tids[i], NULL);

    scheduler_destroy();
}

static void imprimir_tabla(const char *nombre, Camion camiones[]) {
    printf("\n=== Resultados: %s ===\n", nombre);
    printf("%-6s %-10s %-12s %-12s %-12s %-12s\n",
           "ID", "Prioridad", "Burst", "T.Espera", "T.Retorno", "T.Inicio");
    printf("--------------------------------------------------------------\n");

    double suma_espera = 0, suma_retorno = 0;
    for (int i = 0; i < NUM_CAMIONES; i++) {
        Camion *c = &camiones[i];
        double espera   = c->tiempo_inicio - c->tiempo_llegada;
        double retorno  = c->tiempo_fin    - c->tiempo_llegada;
        suma_espera  += espera;
        suma_retorno += retorno;
        printf("C%-5d %-10d %-12d %-12.2f %-12.2f %-12.2f\n",
               c->id, c->prioridad, c->burst, espera, retorno, c->tiempo_inicio);
    }
    printf("--------------------------------------------------------------\n");
    printf("Promedio Espera: %.2f s   Promedio Retorno: %.2f s\n",
           suma_espera / NUM_CAMIONES, suma_retorno / NUM_CAMIONES);
}

int main(void) {
    sincronizacion_init();
    logger_init();

    Camion   camiones_fifo[NUM_CAMIONES], camiones_rr[NUM_CAMIONES];
    pthread_t tids[NUM_CAMIONES];

    /* ── Simulación FIFO ── */
    printf("\n========== SIMULACION FIFO ==========\n");
    modo_algoritmo = FIFO;
    ejecutar_simulacion(FIFO, camiones_fifo, tids);

    /* ── Simulación Round Robin ── */
    printf("\n========== SIMULACION ROUND ROBIN (quantum=%ds) ==========\n", QUANTUM);
    modo_algoritmo = ROUND_ROBIN;
    ejecutar_simulacion(ROUND_ROBIN, camiones_rr, tids);

    /* ── Tabla comparativa ── */
    imprimir_tabla("FIFO",        camiones_fifo);
    imprimir_tabla("ROUND ROBIN", camiones_rr);

    logger_destroy();
    sincronizacion_destroy();
    return 0;
}
