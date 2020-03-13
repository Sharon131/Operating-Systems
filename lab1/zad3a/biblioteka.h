#ifndef BIBLIOTEKA
#define BIBLIOTEKA

#include <stdbool.h>

typedef struct operation_unit {
    char** operations;
    int size;

} operation_unit;

typedef struct main_table{
    char** files_seq;
    int files_pairs_no;
    operation_unit** table;
    bool* used_units;
    int units_no;
} main_table;

void MT_createTable(main_table* mt, int size);

void MT_defineFiles(main_table* mt, char** file_names, int files_no);
void MT_comparePairFromDefinedFiles(main_table* mt, int pair_no);

int MT_createOperationUnitFor(main_table* mt, int file_pair_no);
int MT_getOperationsCounter(operation_unit* op);

void MT_deleteUnit(main_table* mt, int unit_no);
void MT_deleteOperation(main_table* mt, int unit_no, int op_no);

#endif




