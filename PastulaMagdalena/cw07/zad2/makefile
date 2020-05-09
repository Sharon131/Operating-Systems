# Makefile

all: clear main receiver packer sender

main:
	gcc -Wall main.c -o main -lpthread -lrt

receiver:
	gcc -Wall receiver.c -o receiver -lpthread -lrt

packer:
	gcc -Wall packer.c -o packer -lpthread -lrt

sender:
	gcc -Wall sender.c -o sender -lpthread -lrt

clear:
	rm -f main receiver packer sender