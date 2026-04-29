#ifndef LOGGER_H
#define LOGGER_H

void logger_init(void);
void logger_destroy(void);
void log_evento(int camion_id, const char *estado, const char *mensaje);

#endif /* LOGGER_H */
