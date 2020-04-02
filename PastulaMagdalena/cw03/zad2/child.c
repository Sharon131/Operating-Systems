#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "matrix.h"

int get_col_to_multiply(char* comm_file, int max_col_no);

/*void write_result_to_common() {

}

void write_result_to_sep() {

}*/

void write_at_line_end(int fp, int to_write) {
    //int fp = open(exfile, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO);

    char read_char = 0;
    int code = read(fp, &read_char, 1);
    int indx = 0;

    printf("Read chars: \n");
    while (code > 0 && read_char != '\n') {
        read(fp, &read_char, 1);
        printf("%c", read_char);
        indx++;
    }

    printf("\nRead chars end \n");
    /*while (code >0 && indx <= row_no+1) {
        code = read(fp, &read_char, 1);    
        if (code <= 0  || read_char == '\n') {
            indx++;
        }
    }*/

    //jak nie działa to względem początku pliku, dodać liczenie
    if (code > 0) {
        lseek(fp,-1, SEEK_CUR);
    }
    
    int buff_siz = 5;
    char* buff_ch = calloc(buff_siz, sizeof(char));
    snprintf(buff_ch, buff_siz, "%d \n", to_write);    
    
    write(fp, buff_ch, buff_siz);

    //close(fp);
}

void write_result_to_file(struct matrix* m, char* exfile, int col_no, char* mode) {

    if (strcmp(mode, "comm") == 0) {
        int fp = open(exfile, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO);
        //to be added
        close(fp);
    } else if (strcmp(mode, "sep") == 0) {
        int fp = open(exfile, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO);
        
        write_at_line_end(fp, col_no);

        for (int i=0;i<m->rows_no;i++) {
            write_at_line_end(fp, m->vals[i][0]);
        }

        close(fp);
    }
}

int main(int argc, char** argv) {
    //setrlimit();
    char* mfile1 = argv[1];
    char* mfile2 = argv[2];
    char* exfile;
    char* id = argv[4];
    char* mode = argv[5];
    
    char* comm_file = "comm.txt";

    if (strcmp(mode, "comm")==0) {
        exfile = argv[3];
    } else if (strcmp(mode, "sep")==0) {
        exfile = calloc(10,sizeof(char));
        snprintf(exfile, 10, "exf%s.txt", id);
    }
    
    printf("Program 'child', pid: %d\n", (int)getpid());
    printf("Args:\n%s\n%s\n%s\n", mfile1, mfile2, exfile);

    struct matrix* matrix1 = read_matrix_from(mfile1);
    struct matrix* matrix2 = read_matrix_from(mfile2);

    //print_matrix(matrix1);
    //print_matrix(matrix2);
    struct matrix* result = multiply(matrix1, matrix2);
    print_matrix(result);

    int multiplies_no = 0;

    int col_no = get_col_to_multiply(comm_file, matrix2->cols_no);

    //prepare exfile
    int fp = open(exfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    close(fp);
    
    //pierwszyw warunek while'a: czy czas nie został przekroczony dodać potem
    while (col_no < matrix2->cols_no) {
        struct matrix* col = get_product_col(matrix1, matrix2, col_no);
        printf("Kolumna nr: %d\n", col_no);
        print_matrix(col);
        //write result to file 
        write_result_to_file(col, exfile, col_no, mode);
        multiplies_no++;
        col_no = get_col_to_multiply(comm_file, matrix2->cols_no);   
    }

    printf("Ending child process pid %d with %d multiplications\n", (int)getpid(), multiplies_no); 

    free(matrix1);
    free(matrix2);

    return multiplies_no;
}

int get_col_to_multiply(char* comm_file, int max_col_no) {
    int fp = open(comm_file, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO); //O_APPEND ??
    flock(fp, LOCK_EX);
    char buff[1];
    int indx = 0;

    while (read(fp, buff, 1) > 0) {
        if (buff[0] == '\n') {
            indx++;
        }
    }

    if (indx >= max_col_no) {
        return indx;
    }
    
    lseek(fp, 0, SEEK_END);
    int buff_siz = 5;
    char* buff_ch = calloc(buff_siz, sizeof(char));
    snprintf(buff_ch, buff_siz, "%d\n", indx+1);    
    write(fp, buff_ch, buff_siz);

    free(buff_ch);
    flock(fp, LOCK_UN);
    close(fp);

    return indx;
}
