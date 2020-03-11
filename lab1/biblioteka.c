/* Zadanie 1 i 2 są niezależne 
    - w pierwszym trzeba napisać libkę
    - w drugim ją przetestować programem
    Zatem nie ma większego znaczenia, jak biblioteka 
    będzie zaimplementowana, bo całe jej użyie i testowanie
    odbędzie się w zadaniu 2.
*/
#include "biblioteka.h"
#include <stdlib.h>

void MT_createTable(main_table* mt, int size) {
    mt->table = calloc(size, sizeof(operation_unit*));
    mt->used_units = calloc(size, sizeof(bool));
    mt->table_size = size;
}

void MT_defineFiles(char** file_names) {
    ;
}


void MT_compareDefinedFiles(void) {
    system("diff > diff_out.txt"); //concat with files names
}

void MT_compare_pairs(char** files, int size) {

    for (int i=0;i<size; i+=2) {
        char* file1 = files[0]; //????
        char* file2 = files[1]; //correct this


    }
}

int MT_createOperationUnit(void) {
    return 0;
}

int MT_getOperationsCounter(operation_unit* op) {
    return op->size;
}

void MT_deleteUnit(main_table* mt, int unit_no) {
    mt->used_units[unit_no] = false;
}

void MT_deleteOperation(main_table* mt, int unit_no, int op_no) {

}


