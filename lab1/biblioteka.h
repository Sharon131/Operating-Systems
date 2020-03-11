#ifndef BIBLIOTEKA
#define BIBLIOTEKA

#include <stdbool.h>

typedef struct operation_unit {
    char** operations;
    int size;

} operation_unit;

typedef struct main_table{
    operation_unit** table;
    bool* used_units;
    int table_size;
} main_table;

void MT_createTable(main_table* table, int size);
void MT_defineFiles(char** file_names);

void MT_compareDefinedFiles(void);
int MT_createOperationUnit(void);

int MT_getOperationsCounter(operation_unit* op);

void MT_deleteUnit(main_table* mt, int unit_no);
void MT_deleteOperation(main_table* mt, int unit_no, int op_no);

#endif




