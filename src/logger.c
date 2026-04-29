#include "logger.h"
#include "sincronizacion.h"
#include <stdio.h>
#include <time.h>

static FILE *archivo_log = NULL;

void logger_init(void) {
    archivo_log = fopen("terminal.log", "w");
    if (!archivo_log) {
        perror("fopen log");
        archivo_log = stdout;
    }
    fprintf(archivo_log, "%-6s %-12s %-14s %s\n",
            "ID", "ESTADO", "TIEMPO", "MENSAJE");
    fprintf(archivo_log, "----------------------------------------------\n");
}

void logger_destroy(void) {
    if (archivo_log && archivo_log != stdout)
        fclose(archivo_log);
}

void log_evento(int camion_id, const char *estado, const char *mensaje) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double t = ts.tv_sec + ts.tv_nsec / 1e9;

    pthread_mutex_lock(&mutex_log);

    fprintf(archivo_log, "C%-5d %-12s %-14.4f %s\n",
            camion_id, estado, t, mensaje);
    fflush(archivo_log);

    printf("C%-5d %-12s %-14.4f %s\n", camion_id, estado, t, mensaje);

    pthread_mutex_unlock(&mutex_log);
}
