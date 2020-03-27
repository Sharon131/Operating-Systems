#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include "macierz.h"

void read_matrix_from(char* file_name) {
    FILE* fp = fopen(file_name, "r");

    char* buff = NULL;
    int buff_size = 0;
    getline(&buff,&buff_size,fp);

    int** matrix = calloc(0,sizeof(int*));

    fclose(file_name);
}

void multiply() {

}

int main(int argc, char** argv) {
    //jako argumenty nazwy plików z macierzami już
    //a nie początkowy plik
    //działanie procesów potomnych
    //otwieranie plików z macierzami i odczytanie ich

    char* mfile1 = argv[1];
    char* mfile2 = argv[2];
    char* exfile = argv[3];

    printf("Program 'child', pid: %d\n", (int)getpid());
    printf("Args:\n%s%s%s\n", argv[0], argv[1], argv[2]);

    //zamknięte plików z macierzami
    return 0;
}
