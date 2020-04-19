CFLAGS=-std=c11 -g -static

run:
	clang -o main main.c token.c parse.c

test: run
	./test.sh

clean:
	rm tmp* main

.PHONY: test clean
