#include "biblioteka.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILE_BUFF_SIZE  1000000

char* temp_file_name = "diff_out.txt";

static void MT_createNewOperationUnit(int op_no) {
    operation_unit* op = calloc(1, sizeof(operation_unit));
    op->size = op_no;
    op->operations = calloc(op_no, sizeof(char*));
}

static int MT_findFreeUnitSlot(main_table* mt) {

    for (int i=0;i<mt->units_no;i++) {
        if (!mt->used_units[i]) {
            return i;
        }
    }
    return -1;
}

void MT_createTable(main_table* mt, int size) {
    mt->files_seq = calloc(size, sizeof(char*));
    mt->table = calloc(size, sizeof(operation_unit*));
    mt->used_units = calloc(size, sizeof(bool));
    mt->units_no = size;
}

void MT_defineFiles(main_table* mt, char** file_names, int files_no) {

    for (int i=0;i<files_no && i<mt->units_no;i+=2) {
        
        char* file1 = file_names[i];
        char* file2 = file_names[i+1];
        int seq_len = strlen(file1) + strlen(file2) + 2;

        mt->files_seq[i/2] = calloc(seq_len, sizeof(char*));
        mt->files_seq[i/2] = strcat(mt->files_seq[i/2], file1);
        mt->files_seq[i/2][strlen(file1)] = ' ';
        mt->files_seq[i/2] = strcat(mt->files_seq[i/2], file2);
    }
}


void MT_comparePairFromDefinedFiles(main_table* mt, int pair_no) {
    char* command = calloc(21+strlen(mt->files_seq[pair_no]), sizeof(char));
    command = strcat(command, "diff ");
    command = strcat(command, mt->files_seq[pair_no]);
    command = strcat(command, " > diff_out.txt");
    int code = system(command);

    if (code == -1) {
        printf("There was a mistake while running command diff");
        exit(-1);
    } 
}

int MT_createOperationUnitForLastPair(main_table* mt) {
    int fd = open(temp_file_name, O_RDONLY);

	char* buff = calloc(FILE_BUFF_SIZE, sizeof(char));
	int bytes_read = read(fd, buff, FILE_BUFF_SIZE);

    if (bytes_read == -1) {
        printf("There was a problem while attempting to read diff_out.txt file.\n");
    } else if (bytes_read == 0) {
        printf("File diff_out.txt is empty.");
    }

    int operations_no = 1;
    for (int i=1;i<bytes_read;i++) {
        if (buff[i-1]==10 && buff[i] >= '0' && buff[i] <='9') {
            operations_no++;
        }
    }
    //create operation_unit
    int unit_no = MT_findFreeUnitSlot(mt);
    if (unit_no==-1){
        printf("No place in main table to add new block\n");
        exit(-1);
    }

    mt->table[unit_no] = calloc(1, sizeof(operation_unit));
    mt->table[unit_no]->size = operations_no;
    mt->table[unit_no]->operations = calloc(operations_no, sizeof(char*));
    mt->used_units[unit_no] = true;

    int op_no = 0;
    for (int i=0;i<bytes_read;op_no++) {
        //parse content of temp file
        int op_size = 0;
        while (i+op_size<bytes_read && (buff[i+op_size]!= 10 || buff[i+op_size+1] < '0' || buff[i+op_size+1] >'9')) {
            op_size++;
            //i++;
        }
        char* operation = calloc(op_size+1, sizeof(char));  
        operation = strncat(operation, buff+i, op_size);
        mt->table[unit_no]->operations[op_no] = operation;
        i+=op_size+1;
    }

    close(fd);

    return unit_no;
}

int MT_getOperationsCounter(main_table* mt, int unit_no) {
    if (mt != NULL){
        if (mt->table[unit_no] != NULL){
            return mt->table[unit_no]->size;
        }
    }
    return 0;
}

void MT_deleteUnit(main_table* mt, int unit_no) {
    if (mt!=NULL && mt->used_units[unit_no]) {
        mt->used_units[unit_no] = false;
        int op_no = mt->table[unit_no]->size;
        for (int i=0;i<op_no;i++) {
            if (mt->table[unit_no]->operations[i]!=NULL){
                free(mt->table[unit_no]->operations[i]);    
            }
        }
        free(mt->table[unit_no]->operations);
        free(mt->table[unit_no]);
        mt->used_units[unit_no] = false;
    }
}

void MT_deleteOperation(main_table* mt, int unit_no, int op_no) {
    if (mt!=NULL && mt->used_units[unit_no]) {
        if (mt->table[unit_no]->operations[op_no] != NULL) {
            free(mt->table[unit_no]->operations[op_no]);
        }
    }
}


