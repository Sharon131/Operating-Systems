# Makefile
LIB_TYPE=static_library
SLIB_FLAGS=-lbiblioteka
DLIB_FLAGS=-Wl,-rpath=.-lbiblioteka

all: main

test: test_create_table test_compare_pairs_little

test_create_table: main
	./test 5000000 compare_pairs 1 test_mid1.txt test_mid2.txt

test_compare_pairs_little: main
	./test 5000 compare_pairs_n_times 1 5 test_mid1.txt test_mid2.txt

test_compare_pairs_midd: main
	./test 5000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt

test_compare_pairs_alot: main
	./test 5000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt

test_compare_similar_files: main
	./test 5000 compare_pairs_ntimes 1 5000 test_sim1.txt test_sim2.txt

test_compare_alittle_similar_files: main
	./test 5000 compare_pairs_ntimes 1 5000 test_mid1.txt test_mid2.txt

test_compare_not_similar_files: main
	./test 5000 compare_pairs_ntimes 1 5000 test_diff1.txt test_diff2.txt

test_add_small_blocks: main
	./test 5000 compare_pairs 1 test_small1.txt test_small2.txt test_add_block 1000

test_add_mid_blocks: main
	./test 5000 compare_pairs 1 test_sim1.txt test_sim2.txt test_add_block 1000

test_add_big_blocks: main
	./test 5000 compare_pairs 1 test_mid1.txt test_mid2.txt test_add_block 1000

test_remove_small_blocks: main
	./test 5000 compare_pairs 1 test_small1.txt test_small2.txt test_add_block 1000 remove_nfirst_blocks 1000

test_remove_mid_blocks: main
	./test 5000 compare_pairs 1 test_sim1.txt test_sim2.txt test_add_block 1000 remove_nfirst_blocks 1000

test_remove_big_blocks: main
	./test 5000 compare_pairs 1 test_mid1.txt test_mid2.txt test_add_block 1000 remove_nfirst_blocks 1000

test_add_and_delete_blocks: main
	./test 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100

main: clean static_library shared_library
	gcc -Wall test.c -L. -lbiblioteka -o test_static
	#gcc -Wall test.c -L. -Wl,-rpath=.-lbiblioteka -o test_shared

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

shared_library:
	gcc -fPIC -shared biblioteka.c -o libiblioteka.so

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o
clean: 
	rm -f main *.o *.a *.so
