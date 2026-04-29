CC     = gcc
CFLAGS = -Wall -Wextra -pthread -g
INCLUDES = -Iinclude

SRC = src/main.c \
      src/camion.c \
      src/scheduler.c \
      src/sincronizacion.c \
      src/logger.c

OBJ = $(SRC:.c=.o)
TARGET = terminal

# Compilar todo
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Compilar cada .c en su .o
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar compilados
clean:
	rm -f src/*.o $(TARGET)

# Correr el programa
run: all
	./$(TARGET)

.PHONY: all clean run
