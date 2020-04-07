# Makefile

all: clean main child check paste

main: clean child paste
	gcc -Wall main.c -o main

child: matrix
	gcc -Wall child.c -L. -lmatrix -o child

check: matrix
	gcc -Wall check.c -L. -lmatrix -o check

paste:
	gcc -Wall paste_sep.c -L. -lmatrix -o paste_sep

matrix:
	gcc -Wall -c matrix.c -o matrix.o
	ar rcs libmatrix.a matrix.o

clean:
	rm -f *.o main child check *.o
