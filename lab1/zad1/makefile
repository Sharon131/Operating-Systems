# Makefile

all: clean static_library shared_library

shared_library:
	gcc -fPIC -shared biblioteka.c -o libbilioteka.so

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o
clean: 
	rm -f *.o
