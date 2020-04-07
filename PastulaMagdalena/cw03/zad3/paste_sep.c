#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include "matrix.h"

int get_result_rows_no(char* mfile);
int get_result_cols_no(char* mfile);

char** allocate_sep_files(int children_no);

void prepare_exfile(char* exfile, int rows_no, int cols_no);

int check_if_col_available(char** sep_files, int children_no, int col_indx);

void write_first_col_to_exfile(char** sep_files, char* exfile, int file_no);
void add_column_to_exfile(char** sep_files, char* exfile, int file_no);


int main(int argc, char** argv) {

    char* exfile = argv[1];
    int children_no = atoi(argv[2]);
    char* mfile1 = argv[3];
    char* mfile2 = argv[4];

    int res_rows_no = get_result_rows_no(mfile1);
    int res_cols_no = get_result_cols_no(mfile2);
    
    int indx = 0;
    
    char** sep_files = allocate_sep_files(children_no);
    prepare_exfile(exfile, res_rows_no, res_cols_no);

    int fp;
    int children_pending_no = children_no;
    while (indx < res_cols_no) {
        int file_no = check_if_col_available(sep_files, children_no, indx);

        if (indx == 0 && file_no >=0) {
            write_first_col_to_exfile(sep_files, exfile, file_no);   
            indx++;
        } 
        else if (file_no >= 0) {
            add_column_to_exfile(sep_files, exfile, file_no);
            indx++;
        }

        if (children_pending_no > 0) {
            fp = open("sep_comm.txt", O_RDONLY);
            char buff[10];
            read(fp, buff, 10);
            children_pending_no = atoi(buff);
            close(fp);
        }
        
        if (children_pending_no == 0 && file_no < 0) {
            break;
        }
    }
    
    return 0;
}


int get_result_rows_no(char* mfile) {
    struct matrix* m1 = read_matrix_from(mfile);
    int res_rows_no = m1->rows_no;
    free_matrix(m1);
    
    return res_rows_no;
}

int get_result_cols_no(char* mfile) {
    struct matrix* m2 = read_matrix_from(mfile);

    int res_cols_no = m2->cols_no;
    free_matrix(m2);

    return res_cols_no;
}

char** allocate_sep_files(int children_no) {
    char** sep_files = calloc(children_no, sizeof(char*));

    for (int i=0;i<children_no;i++) {
        sep_files[i] = calloc(10, sizeof(char));
        snprintf(sep_files[i], 10, "exf%d.txt", i);
    }

    return sep_files;
}

void prepare_exfile(char* exfile, int rows_no, int cols_no) {
    int fd = open(exfile, O_WRONLY);
    char buff_ch[20];
    snprintf(buff_ch, 20, "%d %d\n", rows_no, cols_no);
    write(fd, buff_ch, strlen(buff_ch));
    close(fd);
}

int check_if_col_available(char** sep_files, int children_no, int col_indx) {
    int i;
    int col = -10;
    for (i=0;i<children_no && col!=col_indx;i++) {
        int fp = open(sep_files[i], O_RDONLY);
        flock(fp, LOCK_EX);
        lseek(fp, 0, SEEK_SET);
        
        char buff[10];

        read(fp, buff, 10);

        if (buff[0] != 0) {
            col = atoi(buff);
        }

        flock(fp, LOCK_UN);
        close(fp);
    }

    if (col == col_indx) {
        return i-1;
    } else {
        return -10;
    }
}

void write_first_col_to_exfile(char** sep_files, char* exfile, int file_no) {
    
    int fp_ex = open(exfile, O_WRONLY);
    int fp_sep = open(sep_files[file_no], O_RDONLY);
    lseek(fp_ex, 0, SEEK_END);

    char buff[1];
    buff[0] = 0;
    while (read(fp_sep, buff, 1) != 0 && buff[0] != '\n') {
        ;
    }

    while (read(fp_sep, buff, 1) != 0) {
        write(fp_ex, buff, 1);
    }

    close(fp_sep);
    close(fp_ex);

    //inform child process it can write another column
    fp_sep = open(sep_files[file_no], O_RDWR);
    flock(fp_sep, LOCK_EX);

    buff[0] = 0;
    write(fp_sep, buff, 1);

    flock(fp_sep, LOCK_UN);
    close(fp_sep);
}

void add_column_to_exfile(char** sep_files, char* exfile, int file_no) {
    int fp = open(sep_files[file_no], O_WRONLY);
    flock(fp, LOCK_EX);
    
    char to_write = ' ';
    write(fp, &to_write, 1);

    flock(fp, LOCK_UN);
    close(fp);

    //paste
    char command[50];
    strcpy(command, "paste ");
    strcpy(command + strlen(command), exfile);
    strcpy(command + strlen(command), " ");
    strcpy(command + strlen(command), sep_files[file_no]);
    strcpy(command + strlen(command), " > tmp2.txt");
    system(command);

    //paste back
    strcpy(command, "paste tmp2.txt > ");
    strcpy(command + strlen(command), exfile);
    system(command);
    
    //inform process it can add another column
    fp = open(sep_files[file_no], O_WRONLY | O_TRUNC);
    flock(fp, LOCK_EX);
    
    to_write = 0;
    write(fp, &to_write, 1);
    
    flock(fp, LOCK_UN);
    close(fp);

}
