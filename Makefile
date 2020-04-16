CFLAGS=-std=c11 -g -static

run:
	clang -o main main.c

test: run
	./test.sh

clean:
	rm tmp* main

.PHONY: test clean
