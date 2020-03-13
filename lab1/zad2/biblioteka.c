#include "biblioteka.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILE_BUFF_SIZE  100

char* temp_file_name = "diff_out.txt";

static void MT_createNewOperationUnit(int op_no) {
    operation_unit* op = calloc(1, sizeof(operation_unit));
    op->size = op_no;
    op->operations = calloc(op_no, sizeof(char*));
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

        mt->files_seq[i] = calloc(seq_len, sizeof(char*));
        mt->files_seq[i] = strcat(mt->files_seq[i], file1);
        mt->files_seq[i][strlen(file1)] = ' ';
        mt->files_seq[i] = strcat(mt->files_seq[i], file2);
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

int MT_createOperationUnitFor(main_table* mt, int file_pair_no) {
    int fd = open(temp_file_name, O_RDONLY);

	char* buff = calloc(FILE_BUFF_SIZE, sizeof(char));
	int bytes_read = read(fd, buff, FILE_BUFF_SIZE);
    
    if (bytes_read == -1) {
        printf("There was a problem while attempting to read diff_out.txt file.\n");
    } else if (bytes_read == 0) {
        printf("File diff_out.txt is empty.");
    }

    close(fd);

    int operations_no = 0;
    for (int i=1;i<bytes_read;i++) {
        if ( buff[i]!='<' && buff[i]!='>' && buff[i]!='-' && buff[i-1]==10) {
            operations_no++;
        }
    }
    //create operation_unit
    int unit_no = file_pair_no-1;
    mt->table[unit_no] = calloc(1, sizeof(operation_unit));
    mt->table[unit_no]->size = operations_no;
    mt->table[unit_no]->operations = calloc(operations_no, sizeof(char*));
    mt->used_units[unit_no] = true;

    int operation_no = 0;
    for (int i=0;i<bytes_read-1;operation_no++) {
        //parse content of temp file
        int operation_size = 0;
        while (buff[i+1]!= 10 || buff[i]=='<' || buff[i]=='>' || buff[i]=='-') {
            operation_size++;
            i++;
        }
        char* operation = calloc(operation_size+1, sizeof(char));  
        operation = strncat(operation, buff+i, operation_size);
        mt->table[unit_no]->operations[operation_no] = operation;
        i+=operation_size;
    }


    return unit_no;
}

int MT_getOperationsCounter(operation_unit* op) {
    return op->size;
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
            printf("Free: %s \n", mt->table[unit_no]->operations[op_no]);
            free(mt->table[unit_no]->operations[op_no]);
        }
    }
}


