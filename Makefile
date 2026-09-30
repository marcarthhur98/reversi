CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Werror
CORE = reversi.c search.c

all: reversi
reversi: $(CORE) main.c reversi.h
	$(CC) $(CFLAGS) $(CORE) main.c -o reversi
test: $(CORE) tests.c reversi.h
	$(CC) $(CFLAGS) $(CORE) tests.c -o tests
	./tests
benchmark: $(CORE) benchmark.c reversi.h
	$(CC) $(CFLAGS) $(CORE) benchmark.c -o benchmark
	./benchmark
.PHONY: all test benchmark
