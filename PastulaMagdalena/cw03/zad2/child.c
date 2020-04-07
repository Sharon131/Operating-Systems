#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "matrix.h"

int get_col_to_multiply(char* comm_file, int max_col_no);

bool is_time_exceeded(clock_t time_start, float max_time);

void write_result_to_common(struct matrix* m, char* exfile, int col_no);
void write_result_to_sep(struct matrix* m, char* exfile, int col_no);

void write_result_to_file(struct matrix* m, char* exfile, int col_no, char* mode);

void write_result_to_sep(struct matrix* m, char* exfile, int col_no) {
    char buff[1];
    buff[0] = 10;

    while (buff[0] != 0) {
        int fp = open(exfile, O_RDONLY);
        flock(fp, LOCK_EX);

        read(fp, buff, 1);

        flock(fp, LOCK_UN);
        close(fp);
    }
    
    int fp = open(exfile, O_RDWR | O_TRUNC);
    flock(fp, LOCK_EX);

    //write col_no to file, then
    //write result to file
    char to_write[10];
    snprintf(to_write, 10, "%d\n", col_no);
    write(fp, to_write, strlen(to_write));

    for (int i=0;i<m->rows_no;i++) {
        if (i!=m->rows_no-1) {
            snprintf(to_write, 10, "%d\n", m->vals[i][0]);
        } else {
            snprintf(to_write, 10, "%d", m->vals[i][0]);
        }
        write(fp, to_write, strlen(to_write));
    }

    flock(fp, LOCK_UN);
    close(fp);
}

// not working
/*void write_at_line_end(int fp, int to_write) {
    //int fp = open(exfile, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO);

    char read_char = 0;
    int code = read(fp, &read_char, 1);
    int indx = 0;

    printf("Read chars: \n");
    while (code != 0 &&  read_char != '\n') {
        printf("%c ", read_char);
        code = read(fp, &read_char, 1);
        indx++;
    }
    printf("%c ", read_char);

    printf("\nRead chars end \n");
    printf("Code (no read bytes): %d\n", code);
    printf("Read char: %d\n", (int)read_char);
    //while (code >0 && indx <= row_no+1) {
    //    code = read(fp, &read_char, 1);    
    //    if (code <= 0  || read_char == '\n') {
    //        indx++;
    //    }
    //}

    //jak nie działa to względem początku pliku, dodać liczenie
    if (read_char == '\n') {
        lseek(fp,-1, SEEK_CUR);
    }
    
    int buff_siz = 10;
    char* buff_ch = calloc(buff_siz, sizeof(char));
    snprintf(buff_ch, buff_siz, "%d \n", to_write);    
    
    write(fp, buff_ch, buff_siz);
    //lseek(fp, 1, SEEK_CUR);

    //close(fp);
}

void print_file(char* file_name) {
    int fp = open(file_name, O_RDWR);
    char buff[1];

    printf("Printing file:\n");
    while (read(fp, buff, 1) != 0) {
        printf("%c", buff[0]);
    }

    printf("\nEnded printing\n");

    close(fp);
}

// not needed??
void write_at_end_of(char* exfile, int to_write, int row_no) {
    int line_no = row_no+1;

    int fp = open(exfile, O_RDWR | O_CREAT, S_IRWXU| S_IRWXO);
    lseek(fp, 0, SEEK_SET);

    char read_char[1];
    int code = read(fp, &read_char, 1);
    int indx = 0;
    int read_chars_no = 1;

    if (code != 0) {
        //printf("EOF\n");

        printf("Writing to file: %d at row: %d\n", to_write, line_no);
        while (indx <= line_no) {
            printf("%d ", read_char[0]);
            code = read(fp, &read_char, 1);
            
            if (read_char[0] == '\n') { // || (code == 0 && indx == line_no-1)) {
                indx++;
            }

            read_chars_no++;
        }
        printf("Last char: %d \n", read_char[0]);
        printf("Line index: %d\n", indx);
            
        //if (code == 0 && indx==line_no-1) {
        //    lseek(fp,0, SEEK_END);
        //} else {
            lseek(fp,read_chars_no-1, SEEK_SET);
        //}
        
    }
    
    int buff_siz = 10;
    char* buff_ch = calloc(buff_siz, sizeof(char));
    snprintf(buff_ch, buff_siz, "%d \n", to_write);    
    
    printf("Written string:\n");
    int ind = 0;
    while (buff_ch[ind] != 0) {
        write(fp, buff_ch+ind, 1);        
        printf("%d ", buff_ch[ind]);
        ind++;
    }
    printf("\n");
    
    //buff_ch[ind] = '\n';
    //write(fp, buff_ch+ind, 1);  

    //write(fp, buff_ch, buff_siz);
    //write(fp, "\n", 1);

    print_file(exfile);

    close(fp);
}*/


int main(int argc, char** argv) {
    //setrlimit();
    clock_t start_time = times(NULL);

    char* mfile1 = argv[1];
    char* mfile2 = argv[2];
    char* exfile;
    char* id = argv[4];
    float max_time = atof(argv[5]);
    char* mode = argv[6];
    
    char* comm_file = "comm.txt";

    if (strcmp(mode, "comm")==0) {
        exfile = argv[3];
    } else if (strcmp(mode, "sep")==0) {
        exfile = calloc(10,sizeof(char));
        snprintf(exfile, 10, "exf%s.txt", id);
    }

    struct matrix* matrix1 = read_matrix_from(mfile1);
    struct matrix* matrix2 = read_matrix_from(mfile2);

    struct matrix* result = multiply(matrix1, matrix2);
    //print_matrix(result);

    //prepare exfile
    if (strcmp(mode, "sep")==0) {
        int fp = open(exfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
        char to_write = 0;
        write(fp, &to_write, 1);
        close(fp);
    }

    int multiplies_no = 0;
    int col_no = get_col_to_multiply(comm_file, matrix2->cols_no);
    
    while (col_no < matrix2->cols_no) {
        struct matrix* col = get_product_col(matrix1, matrix2, col_no);
        
        write_result_to_file(col, exfile, col_no, mode);
        multiplies_no++;
        
        if (is_time_exceeded(start_time, max_time)) {
            printf("Ending child process pid %d with %d multiplication(s) before time exceeded.\n", (int)getpid(), multiplies_no); 
            return multiplies_no;
        }
        col_no = get_col_to_multiply(comm_file, matrix2->cols_no);  
    }

    printf("Ending child process pid %d with %d multiplication(s).\n", (int)getpid(), multiplies_no); 

    free(matrix1);
    free(matrix2);
    free(result);

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

bool is_time_exceeded(clock_t time_start, float max_time) {
    clock_t time_end = times(NULL);
    float time_elapsed = (time_end-time_start)/(float)sysconf(_SC_CLK_TCK);

    return fabs(time_elapsed) > max_time;
}

void write_result_to_common(struct matrix* m, char* exfile, int col_no) {
    
    if (col_no == 0) {
        int fp = open(exfile, O_RDWR);

        flock(fp, LOCK_EX);
        char to_write[50];
        // write rows and cols no
        snprintf(to_write, 50, "%d 1\n", m->rows_no);
        write(fp, to_write, strlen(to_write));

        for (int i=0;i<m->rows_no;i++) {
            if (i != m->rows_no-1) {
                snprintf(to_write, 50, "%d\n", m->vals[i][0]);
            } else {
                snprintf(to_write, 50, "%d", m->vals[i][0]);
            }
            write(fp, to_write, strlen(to_write));
        }

        flock(fp, LOCK_UN);
        close(fp);
    } else {
        int cols_no = -10;
        char buff[10];

        while (cols_no != col_no) {
            int fp = open(exfile, O_RDONLY);

            flock(fp, LOCK_EX);
            lseek(fp, 0, SEEK_SET);
            int indx = 0; //not needed actually

            while (read(fp, buff, 1)==1 && buff[0]!= ' ' && buff[0]!='\t') {
                indx++;
            }

            read(fp, buff, 10);
            cols_no = atoi(buff);
            

            flock(fp, LOCK_UN);
            close(fp);
        }

        // write result to tmp file
        char to_write[50];
        int fp_tmp = open("tmp.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
        flock(fp_tmp, LOCK_EX);

        if ((col_no+1)%10 == 0) {
            snprintf(to_write, 50, " \n");
        } else {
            snprintf(to_write, 50, "\n");
        }
        write(fp_tmp, to_write, strlen(to_write));

        for (int i=0;i<m->rows_no;i++) {
            if (i != m->rows_no-1) {
                snprintf(to_write, 50, "%d\n", m->vals[i][0]);
            } else {
                snprintf(to_write, 50, "%d", m->vals[i][0]);
            }
            write(fp_tmp, to_write, strlen(to_write));
        }

        flock(fp_tmp, LOCK_UN);
        close(fp_tmp);

        // paste to tmp file whole result
        char command[50];
        strcpy(command, "paste ");
        strcpy(command+ strlen(command), exfile);
        strcpy(command + strlen(command), " tmp.txt > tmp2.txt");
        system(command);

        // paste back
        strcpy(command, "paste tmp2.txt > ");
        strcpy(command + strlen(command), exfile);
        system(command);
        
        //update cols no
        int fp = open(exfile, O_WRONLY);
        flock(fp, LOCK_EX);
        
        lseek(fp, 0, SEEK_SET);

        snprintf(to_write, 50, "%d %d", m->rows_no, col_no+1);

        write(fp, to_write, strlen(to_write));

        flock(fp, LOCK_UN);
        close(fp);
    }

}

void write_result_to_file(struct matrix* m, char* exfile, int col_no, char* mode) {

    if (strcmp(mode, "comm") == 0) {
        
        write_result_to_common(m, exfile, col_no);

    } else if (strcmp(mode, "sep") == 0) {
        
        write_result_to_sep(m, exfile, col_no);
    }
}