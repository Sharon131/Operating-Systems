# Makefile
LIB_TYPE=static_library
SLIB_FLAGS=-lbiblioteka
DLIB_FLAGS=-Wl,-rpath=.-lbiblioteka

all: main

test: main test_static test_shared test_dynamic

main: clean static_library shared_library
	gcc -Wall test.c -L. -lbiblioteka -o test_static
	gcc -Wall test.c -L. -Wl,-rpath=. -lbiblioteka -o test_shared
	gcc -Wall test.c -o test_dynamic -ldl -D DLL

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

shared_library:
	gcc -fPIC -shared biblioteka.c -o libbiblioteka.so

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o

clean: 
	rm -f main *.o *.a *.so

test_static: main
	echo 'static library'
	echo 'create table'
	./test_static 50000000 create_table
	echo 'compare pairs'
	./test_static 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt
	./test_static 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt
	./test_static 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt
	echo 'compare similar pairs'
	./test_static 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt
	./test_static 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt
	./test_static 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt
	echo 'adding blocks'
	./test_static 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000
	./test_static 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000
	./test_static 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000
	echo 'add then remove blocks'
	./test_static 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_static 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_static 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000
	echo 'add and delete blocks'
	./test_static 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100

test_shared: main
	echo 'shared library'
	echo 'create table'
	./test_shared 50000000 create_table
	echo 'compare pairs'
	./test_shared 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt
	./test_shared 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt
	./test_shared 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt
	echo 'compare similar pairs'
	./test_shared 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt
	./test_shared 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt
	./test_shared 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt
	echo 'adding blocks'
	./test_shared 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000
	./test_shared 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000
	./test_shared 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000
	echo 'add then remove blocks'
	./test_shared 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_shared 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_shared 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000
	echo 'add and delete blocks'
	./test_shared 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100

test_dynamic: main
	echo 'dynamic library'
	echo 'create table'
	./test_dynamic 50000000 create_table
	echo 'compare pairs'
	./test_dynamic 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt
	./test_dynamic 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt
	./test_dynamic 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt
	echo 'compare similar pairs'
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt
	echo 'adding blocks'
	./test_dynamic 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000
	./test_dynamic 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000
	./test_dynamic 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000
	echo 'add then remove blocks'
	./test_dynamic 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_dynamic 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000
	./test_dynamic 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000
	echo 'add and delete blocks'
	./test_dynamic 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100

