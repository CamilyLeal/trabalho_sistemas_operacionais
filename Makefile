
TARGET = formigopolis

SRCS = main.c pessoa.c caixa.c gerente.c

CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lpthread


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)