#ifndef TERMINAL_H
#define TERMINAL_H

#define NUM_MUELLES  3
#define NUM_CAMIONES 8
#define QUANTUM      2

typedef enum {
    FIFO,
    ROUND_ROBIN
} Algoritmo;

typedef enum {
    ESTADO_NUEVO,
    ESTADO_LISTO,
    ESTADO_EJECUCION,
    ESTADO_BLOQUEADO,
    ESTADO_TERMINADO
} EstadoCamion;

typedef struct {
    int          id;
    int          prioridad;
    int          burst;
    int          burst_restante;
    EstadoCamion estado;
    double       tiempo_llegada;
    double       tiempo_inicio;
    double       tiempo_fin;
} Camion;

#endif /* TERMINAL_H */
