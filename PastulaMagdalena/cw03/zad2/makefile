# Makefile

all: clean main child check

main: clean child
	gcc -Wall main.c -o main

child: matrix
	gcc -Wall child.c -L. -lmatrix -o child

check: matrix
	gcc -Wall check.c -L. -lmatrix -o check

matrix:
	gcc -Wall -c matrix.c -o matrix.o
	ar rcs libmatrix.a matrix.o

clean:
	rm -f *.o main child check *.o
