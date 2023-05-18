CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lncurses

pizzaGasRep: pizzaGasRep.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -f pizzaGasRep
