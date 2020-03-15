# Makefile
FILE=report.txt
all: main

optimalization_tests: clean_file
	make tests opt=0
	make tests opt=1
	make tests opt=2
	make tests opt=s

tests: main test_static test_shared test_dynamic

main: clean static_library shared_library
	gcc -Wall test.c -static -L. -lbiblioteka -o test_static -O$(opt)
	gcc -Wall test.c -L. -Wl,-rpath=. -lbiblioteka -o test_shared -O$(opt)
	gcc -Wall test.c -o test_dynamic -ldl -D DLL -O$(opt)

static_library: biblioteka.o
	ar rcs libbiblioteka.a biblioteka.o

shared_library:
	gcc -fPIC -shared biblioteka.c -o libbiblioteka.so -O$(opt)

biblioteka.o:
	gcc -c biblioteka.c -o biblioteka.o -O$(opt)

clean: 
	rm -f main *.o *.a *.so

clean_file:
	rm -f $(FILE)

test_static: main
	echo '--------------------------------' >> $(FILE)
	echo 'static library optimalization -O$(opt)' >> $(FILE)
	echo 'create table of size 50 000 000' >> $(FILE)
	./test_static 50000000 create_table | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare different number of pairs' >> $(FILE)
	echo 'compare 5 pairs' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 50 pairs' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 500 pairs' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare differently similar pairs' >> $(FILE)
	echo 'compare quite similar files' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt | tee -a $(FILE)
	echo 'compare no so similar files' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare not similar files' >> $(FILE)
	./test_static 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add then remove 1000 blocks' >> $(FILE)
	echo 'Small blocks' >> $(FILE)
	./test_static 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Medium blocks' >> $(FILE)
	./test_static 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Big blocks' >> $(FILE)
	./test_static 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add and delete 100 blocks 100 times' >> $(FILE)
	./test_static 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100 | tee -a $(FILE)

test_shared: main
	echo '--------------------------------' >> $(FILE)
	echo 'shared library optimalization -O$(opt)' >> $(FILE)
	echo 'create table of size 50 000 000' >> $(FILE)
	./test_shared 50000000 create_table | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare different number of pairs' >> $(FILE)
	echo 'compare 5 pairs' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 50 pairs' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 500 pairs' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare differently similar pairs' >> $(FILE)
	echo 'compare quite similar files' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt | tee -a $(FILE)
	echo 'compare not so similar files' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare not similar files' >> $(FILE)
	./test_shared 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add then remove 1000 blocks' >> $(FILE)
	echo 'Small blocks' >> $(FILE)
	./test_shared 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Medium blocks' >> $(FILE)
	./test_shared 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Big blocks' >> $(FILE)
	./test_shared 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add and delete 100 blocks 100 times' >> $(FILE)
	./test_shared 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100 | tee -a $(FILE)

test_dynamic: main
	echo '--------------------------------' >> $(FILE)
	echo 'dynamic library optimalization -O$(opt)' >> $(FILE)
	echo 'create table of size 50 000 000' >> $(FILE)
	./test_dynamic 50000000 create_table | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare different number of pairs' >> $(FILE)
	echo 'compare 5 pairs' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 5 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 50 pairs' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 50 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare 500 pairs' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 500 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'compare differently similar pairs' >> $(FILE)
	echo 'compare quite similar files' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_sim1.txt test_sim2.txt | tee -a $(FILE)
	echo 'compare not so similar files' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_mid1.txt test_mid2.txt | tee -a $(FILE)
	echo 'compare not similar files' >> $(FILE)
	./test_dynamic 1000 compare_pairs_ntimes 1 100 test_diff1.txt test_diff2.txt | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add then remove 1000 blocks' >> $(FILE)
	echo 'Small blocks' >> $(FILE)
	./test_dynamic 5000 compare_pairs 1 test_small1.txt test_small2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Medium blocks' >> $(FILE)
	./test_dynamic 5000 compare_pairs 1 test_sim1.txt test_sim2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo 'Big blocks' >> $(FILE)
	./test_dynamic 5000 compare_pairs 1 test_mid1.txt test_mid2.txt add_block 1000 remove_nfirst_blocks 1000 | tee -a $(FILE)
	echo ' ' >> $(FILE)
	echo 'add and delete 100 blocks 100 times' >> $(FILE)
	./test_dynamic 1000 compare_pairs 1 test_mid1.txt test_mid2.txt add_and_delete 100 100 | tee -a $(FILE)

