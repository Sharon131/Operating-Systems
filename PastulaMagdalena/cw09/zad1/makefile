# Makefile

all: clear main

tests: all
	./main 2 10
	./main 3 20

test: all
	./main 2 10

main:
	gcc -Wall main.c -o main -lpthread

clear:
	rm -f main