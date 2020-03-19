# Makefile
FILE=wyniki.txt
all: main

tests: clean_file main
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 1' >> $(FILE)
	./main generate testa.txt 1000000 1
	./main generate testb.txt 1000000000 1
	./main copy testa.txt test1a_sys.txt 1000000 1 sys | tee -a $(FILE)
	./main copy testa.txt test1a_lib.txt 1000000 1 lib | tee -a $(FILE)
	./main copy testa.txt test1b_sys.txt 1000000000 1 sys | tee -a $(FILE)
	./main copy testa.txt test1b_lib.txt 1000000000 1 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 4' >> $(FILE)
	./main generate testa.txt 200 4
	./main generate testb.txt 200 4
	./main copy testa.txt test4a_sys.txt 200 4 sys | tee -a $(FILE)
	./main copy testa.txt test4a_lib.txt 200 4 lib | tee -a $(FILE)
	./main copy testa.txt test4b_sys.txt 200 4 sys | tee -a $(FILE)
	./main copy testa.txt test4b_lib.txt 200 4 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 512' >> $(FILE)
	./main generate testa.txt 200 512
	./main generate testb.txt 200 512
	./main copy testa.txt test512a_sys.txt 200 512 sys | tee -a $(FILE)
	./main copy testa.txt test512a_lib.txt 200 512 lib | tee -a $(FILE)
	./main copy testa.txt test512b_sys.txt 200 512 sys | tee -a $(FILE)
	./main copy testa.txt test512b_lib.txt 200 512 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 1024' >> $(FILE)
	./main generate testa.txt 200 1024
	./main generate testb.txt 200 1024
	./main copy testa.txt test1024a_sys.txt 200 1024 sys | tee -a $(FILE)
	./main copy testa.txt test1024a_lib.txt 200 1024 lib | tee -a $(FILE)
	./main copy testa.txt test1024b_sys.txt 200 1024 sys | tee -a $(FILE)
	./main copy testa.txt test1024b_lib.txt 200 1024 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 4096' >> $(FILE)
	./main generate testa.txt 200 4096
	./main generate testb.txt 200 4096
	./main copy testa.txt test4096a_sys.txt 200 4096 sys | tee -a $(FILE)
	./main copy testa.txt test4096a_lib.txt 200 4096 lib | tee -a $(FILE)
	./main copy testa.txt test4096b_sys.txt 200 4096 sys | tee -a $(FILE)
	./main copy testa.txt test4096b_lib.txt 200 4096 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 8192' >> $(FILE)
	./main generate testa.txt 200 8192
	./main generate testb.txt 200 8192
	./main copy testa.txt test8192a_sys.txt 200 8192 sys | tee -a $(FILE)
	./main copy testa.txt test8192a_lib.txt 200 8192 lib | tee -a $(FILE)
	./main copy testa.txt test8192b_sys.txt 200 8192 sys | tee -a $(FILE)
	./main copy testa.txt test8192b_lib.txt 200 8192 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 1' >> $(FILE)
	./main sort test1a_sys.txt 200 1 sys	| tee -a $(FILE)
	./main sort test1a_lib.txt 200 1 lib	| tee -a $(FILE)
	./main sort test1b_sys.txt 200 1 sys 	| tee -a $(FILE)
	./main sort test1b_lib.txt 200 1 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 4' >> $(FILE)
	./main sort test4a_sys.txt 200 4 sys 	| tee -a $(FILE)
	./main sort test4a_lib.txt 200 4 lib 	| tee -a $(FILE)
	./main sort test4b_sys.txt 200 4 sys 	| tee -a $(FILE)
	./main sort test4b_lib.txt 200 4 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 512' >> $(FILE)
	./main sort test512a_sys.txt 200 512 sys 	| tee -a $(FILE)
	./main sort test512a_lib.txt 200 512 lib 	| tee -a $(FILE)
	./main sort test512b_sys.txt 200 512 sys 	| tee -a $(FILE)
	./main sort test512b_lib.txt 200 512 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 1024' >> $(FILE)
	./main sort test1024a_sys.txt 200 1024 sys 	| tee -a $(FILE)
	./main sort test1024a_lib.txt 200 1024 lib 	| tee -a $(FILE)
	./main sort test1024b_sys.txt 200 1024 sys 	| tee -a $(FILE)
	./main sort test1024b_lib.txt 200 1024 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 4096' >> $(FILE)
	./main sort test4096a_sys.txt 200 4096 sys 	| tee -a $(FILE)
	./main sort test4096a_lib.txt 200 4096 lib 	| tee -a $(FILE)
	./main sort test4096b_sys.txt 200 4096 sys 	| tee -a $(FILE)
	./main sort test4096b_lib.txt 200 4096 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 8192' >> $(FILE)
	./main sort test8192a_sys.txt 200 8192 sys 	| tee -a $(FILE)
	./main sort test8192a_lib.txt 200 8192 lib 	| tee -a $(FILE)
	./main sort test8192b_sys.txt 200 8192 sys 	| tee -a $(FILE)
	./main sort test8192b_lib.txt 200 8192 lib 	| tee -a $(FILE)
	

main: clean
	gcc -Wall main.c -o main

clean:
	rm -f main *.o *.txt

clean_file:
	rm wyniki.txt