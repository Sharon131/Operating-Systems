# Makefile

all: main

main: clean static_library
	#gcc -Wall main.c -o main
	gcc -Wall main.c -L. -lbiblioteka -o main

test:
	gcc -Wall test.c -L../zad1 -lbiblioteka - test

shared_library: biblioteka.o
	gcc -fPIC -shared biblioteka.o -o libbilioteka.so

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o
clean: 
	rm -f main
