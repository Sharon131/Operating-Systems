# Makefile

all: main

main: clean static_library
	gcc -Wall test.c -L. -lbiblioteka -o test

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o
clean: 
	rm -f main *.o *.a *.so
