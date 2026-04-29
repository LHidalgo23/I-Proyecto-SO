#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "terminal.h"

void    scheduler_init(Algoritmo alg);
void    scheduler_encolar(Camion *c);
Camion *scheduler_siguiente(void);
int     scheduler_vacia(void);
void    scheduler_destroy(void);

#endif
