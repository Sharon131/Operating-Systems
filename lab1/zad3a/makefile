# Makefile

test_static: library
	gcc -Wall test.c -L -lbiblioteka -o test
test_shared: library
	gcc -Wall test.c -L -Wl -rpath=. -lbiblioteka -o test
test_dynamic: library
	gcc -Wall test.c -o test
shared_library:
	gcc -fPIC -shared biblioteka.c -o libbiblioteka.so	
static_library: library
	ar rcs libbiblioteka.a biblioteka.o
library:
	gcc -c biblioteka.c -o biblioteka.o
clean:
	rm -f test *.o *.a *.so
