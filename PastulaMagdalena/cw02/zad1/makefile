# Makefile
FILE=wyniki.txt

REC_NO1 = $ `expr 10000000 / $(size)`
REC_NO2 = $ `expr 1000000000 / $(size)`

all: main

tests: clean_file main
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 1' >> $(FILE)
	./main generate testa.txt 1000000 1
	./main generate testb.txt 10000000 1
	./main copy testa.txt test1a_sys.txt 1000000 1 sys | tee -a $(FILE)
	./main copy testa.txt test1a_lib.txt 1000000 1 lib | tee -a $(FILE)
	./main copy testa.txt test1b_sys.txt 10000000 1 sys | tee -a $(FILE)
	./main copy testa.txt test1b_lib.txt 10000000 1 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 4' >> $(FILE)
	./main generate testa.txt 100000 4
	./main generate testb.txt 1000000 4
	./main copy testa.txt test4a_sys.txt 100000 4 sys | tee -a $(FILE)
	./main copy testa.txt test4a_lib.txt 100000 4 lib | tee -a $(FILE)
	./main copy testa.txt test4b_sys.txt 1000000 4 sys | tee -a $(FILE)
	./main copy testa.txt test4b_lib.txt 1000000 4 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 512' >> $(FILE)
	./main generate testa.txt 10000 512
	./main generate testb.txt 100000 512
	./main copy testa.txt test512a_sys.txt 10000 512 sys | tee -a $(FILE)
	./main copy testa.txt test512a_lib.txt 10000 512 lib | tee -a $(FILE)
	./main copy testa.txt test512b_sys.txt 100000 512 sys | tee -a $(FILE)
	./main copy testa.txt test512b_lib.txt 100000 512 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 1024' >> $(FILE)
	./main generate testa.txt 10000 1024
	./main generate testb.txt 100000 1024
	./main copy testa.txt test1024a_sys.txt 10000 1024 sys | tee -a $(FILE)
	./main copy testa.txt test1024a_lib.txt 10000 1024 lib | tee -a $(FILE)
	./main copy testa.txt test1024b_sys.txt 100000 1024 sys | tee -a $(FILE)
	./main copy testa.txt test1024b_lib.txt 100000 1024 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 4096' >> $(FILE)
	./main generate testa.txt 10000 4096
	./main generate testb.txt 20000 4096
	./main copy testa.txt test4096a_sys.txt 10000 4096 sys | tee -a $(FILE)
	./main copy testa.txt test4096a_lib.txt 10000 4096 lib | tee -a $(FILE)
	./main copy testa.txt test4096b_sys.txt 20000 4096 sys | tee -a $(FILE)
	./main copy testa.txt test4096b_lib.txt 20000 4096 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = 8192' >> $(FILE)
	./main generate testa.txt 10000 8192
	./main generate testb.txt 20000 8192
	./main copy testa.txt test8192a_sys.txt 10000 8192 sys | tee -a $(FILE)
	./main copy testa.txt test8192a_lib.txt 10000 8192 lib | tee -a $(FILE)
	./main copy testa.txt test8192b_sys.txt 20000 8192 sys | tee -a $(FILE)
	./main copy testa.txt test8192b_lib.txt 20000 8192 lib | tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 1' >> $(FILE)
	./main generate test1a2_sys.txt 10000 1
	cp test1a2_sys.txt test1a2_lib.txt
	./main generate test1b2_sys.txt 100000 1
	cp test1b2_sys.txt test1b2_lib.txt
	./main sort test1a2_sys.txt 10000 1 sys	| tee -a $(FILE)
	./main sort test1a2_lib.txt 10000 1 lib	| tee -a $(FILE)
	./main sort test1b2_sys.txt 100000 1 sys 	| tee -a $(FILE)
	./main sort test1b2_lib.txt 100000 1 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 4' >> $(FILE)
	./main generate test4a2_sys.txt 100000 4
	cp test4a2_sys.txt test4a2_lib.txt
	./main generate test4b2_sys.txt 1000000 4
	cp test4b2_sys.txt test4b2_lib.txt
	./main sort test4a2_sys.txt 10000 4 sys 	| tee -a $(FILE)
	./main sort test4a2_lib.txt 10000 4 lib 	| tee -a $(FILE)
	./main sort test4b2_sys.txt 100000 4 sys 	| tee -a $(FILE)
	./main sort test4b2_lib.txt 100000 4 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 512' >> $(FILE)
	./main generate test512a2_sys.txt 10000 512
	cp test512a2_sys.txt test512a2_lib.txt
	./main generate test512b2_sys.txt 100000 512
	cp test512b2_sys.txt test512b2_lib.txt
	./main sort test512a2_sys.txt 10000 512 sys 	| tee -a $(FILE)
	./main sort test512a2_lib.txt 10000 512 lib 	| tee -a $(FILE)
	./main sort test512b2_sys.txt 100000 512 sys 	| tee -a $(FILE)
	./main sort test512b2_lib.txt 100000 512 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 1024' >> $(FILE)
	./main generate test1024a2_sys.txt 10000 1024
	cp test1024a2_sys.txt test1024a2_lib.txt
	./main generate test1024b2_sys.txt 100000 1024
	cp test1024b2_sys.txt test1024b2_lib.txt
	./main sort test1024a2_sys.txt 10000 1024 sys 	| tee -a $(FILE)
	./main sort test1024a2_lib.txt 10000 1024 lib 	| tee -a $(FILE)
	./main sort test1024b2_sys.txt 100000 1024 sys 	| tee -a $(FILE)
	./main sort test1024b2_lib.txt 100000 1024 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 4096' >> $(FILE)
	./main generate test4096a2_sys.txt 10000 4096
	cp test4096a2_sys.txt test4096a2_lib.txt
	./main generate test4096b2_sys.txt 100000 4096
	cp test4096b2_sys.txt test4096b2_lib.txt
	./main sort test4096a2_sys.txt 10000 4096 sys 	| tee -a $(FILE)
	./main sort test4096a2_lib.txt 10000 4096 lib 	| tee -a $(FILE)
	./main sort test4096b2_sys.txt 100000 4096 sys 	| tee -a $(FILE)
	./main sort test4096b2_lib.txt 100000 4096 lib 	| tee -a $(FILE)
	echo '--------------------------------' >> $(FILE)
	echo 'Sort file with record size = 8192' >> $(FILE)
	./main generate test8192a2_sys.txt 10000 8192
	cp test8192a2_sys.txt test8192a2_lib.txt
	./main generate test8192b2_sys.txt 100000 8192
	cp test8192b2_sys.txt test8192b2_lib.txt
	./main sort test8192a2_sys.txt 10000 8192 sys 	| tee -a $(FILE)
	./main sort test8192a2_lib.txt 10000 8192 lib 	| tee -a $(FILE)
	./main sort test8192b2_sys.txt 100000 8192 sys 	| tee -a $(FILE)
	./main sort test8192b2_lib.txt 100000 8192 lib 	| tee -a $(FILE)
	
test:
	echo '--------------------------------' >> $(FILE)
	echo 'Copy file with record size = $(size)' >> $(FILE)
	./main generate testa.txt $(REC_NO1) $(size)
	./main generate testb.txt $(REC_NO2) $(size)
	./main copy testa.txt test$(size)a_sys.txt $(REC_NO1) $(size) sys | tee -a $(FILE)
	./main copy testa.txt test$(size)a_lib.txt $(REC_NO1) $(size) lib | tee -a $(FILE)
	./main copy testa.txt test$(size)b_sys.txt $(REC_NO2) $(size) sys | tee -a $(FILE)
	./main copy testa.txt test$(size)b_lib.txt $(REC_NO2) $(size) lib | tee -a $(FILE)
	

main: clean
	gcc -Wall main.c -o main

clean:
	rm -f main *.o *.txt

clean_file:
	rm -f wyniki.txt