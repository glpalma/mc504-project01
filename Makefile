CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lncurses

pizza504: pizza504.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -f pizza504
