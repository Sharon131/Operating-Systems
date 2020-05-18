# Makefile

all: clear main

tests: clear main
	rm -f times.txt
	# 1 thread
	./main 1 sign lena.ascii.pgm sign_out_1.txt
	./main 1 block lena.ascii.pgm block_out_1.txt
	./main 1 interleaved lena.ascii.pgm inter_out_1.txt
	# 2 threads
	./main 2 sign lena.ascii.pgm sign_out_2.txt
	./main 2 block lena.ascii.pgm block_out_2.txt
	./main 2 interleaved lena.ascii.pgm inter_out_2.txt
	# 4 threads
	./main 4 sign lena.ascii.pgm sign_out_4.txt
	./main 4 block lena.ascii.pgm block_out_4.txt
	./main 4 interleaved lena.ascii.pgm inter_out_4.txt
	# 8 threads
	./main 8 sign lena.ascii.pgm sign_out_8.txt
	./main 8 block lena.ascii.pgm block_out_8.txt
	./main 8 interleaved lena.ascii.pgm inter_out_8.txt

test: clear main
	rm -f times.txt
	./main $(threads_no) sign lena.ascii.pgm sign_out.txt

main:
	gcc -Wall main.c -o main -lpthread

clear:
	rm -f main