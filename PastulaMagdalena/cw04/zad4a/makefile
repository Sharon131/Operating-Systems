# Makefile

all: clear lib sender catcher

catcher:
	gcc -Wall catcher.c -L. -lcomm -o catcher

sender:
	gcc -Wall sender.c -L. -lcomm -o sender

lib:
	gcc -c comm.c -o comm.o
	ar rcs libcomm.a comm.o

clear:
	rm -f sender catcher *.o *.a