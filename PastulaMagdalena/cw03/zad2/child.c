#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include "macierz.h"

int main(int argc, char** argv) {
    //jako argumenty nazwy plików z macierzami już
    //a nie początkowy plik
    //działanie procesów potomnych
    //otwieranie plików z macierzami i odczytanie ich

    printf("Program 'child', pid: %d\n", (int)getpid());

    //zamknięte plików z macierzami
    return 0;
}
