#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"

void check_argc(int argc);

void fill_matrix_with_rand(struct matrix* m);
void write_matrix_to_file(struct matrix* m, char* file_name);
void write_to_main_file(char* mfile1, char* mfile2, int indx);

void generate(int pairs_no, int size_min, int size_max);
void test(char* main_file, char* file1, char* file2, char* exfile);


int main(int argc, char** argv) {
    check_argc(argc);

    char* command = argv[1];

    if (strcmp("generate", command) == 0) {
        int pairs_no = atoi(argv[2]);
        int size_min = atoi(argv[3]);
        int size_max = atoi(argv[4]);
        generate(pairs_no, size_min, size_max);
    } else if (strcmp("test", command) == 0) {
        char* file_name = argv[2];
        char *mfile1 = NULL, *mfile2 = NULL, *exfile = NULL;
        size_t mfile1_size = 0, mfile2_size = 0, exfile_size = 0;

        FILE* fp = fopen(file_name,"r");

        int chars_no = getline(&mfile1,&mfile1_size, fp);
        mfile1[chars_no-1] = 0;
        chars_no = getline(&mfile2,&mfile2_size, fp);
        mfile2[chars_no-1] = 0;
        chars_no = getline(&exfile, &exfile_size, fp);
        exfile[chars_no-1] = 0;

        fclose(fp);
        
        test(file_name, mfile1, mfile2, exfile);
    } else {
        printf("Not known command. Exiting.\n");
        exit(-1);
    }

    return 0;
}

void check_argc(int argc) {
    if (argc < 2) {
        printf("Not enough arguments.\n");
        exit(-1);
    }
}

void fill_matrix_with_rand(struct matrix* m) {
    
    for (int i=0;i<m->rows_no;i++) {
        for (int j=0;j<m->cols_no;j++) {
            m->vals[i][j] = rand()%10;
        }
    }
}

void write_matrix_to_file(struct matrix* m, char* file_name) {
    FILE* fp = fopen(file_name, "w");
    
    fprintf(fp, "%d %d\n", m->rows_no, m->cols_no);

    for (int i=0;i<m->rows_no;i++) {

        for (int j=0;j<m->cols_no-1;j++) {
            fprintf(fp, "%d ", m->vals[i][j]);
        }
        fprintf(fp, "%d\n", m->vals[i][m->cols_no-1]);
    }

    fclose(fp);
}

void write_to_main_file(char* mfile1, char* mfile2, int indx) {
    char main_name[20];
    snprintf(main_name, 20, "test%d.txt", indx);

    char exit_name[20];
    snprintf(exit_name, 20, "exit%d.txt", indx);

    FILE* fp = fopen(main_name, "w");

    fprintf(fp, "%s\n", mfile1);
    fprintf(fp, "%s\n", mfile2);
    fprintf(fp, "%s\n", exit_name);

    fclose(fp);
}

void generate(int pairs_no, int size_min, int size_max) {
    int size_interval = size_max-size_min;

    for (int i=0;i<pairs_no;i++) {
        int A_rows_no = rand()%size_interval+size_min;
        int B_cols_no = rand()%size_interval+size_min;
        int A_cols_B_rows_no = rand()%size_interval+size_min;

        struct matrix m1, m2;
        create_matrix(&m1, A_rows_no, A_cols_B_rows_no);
        create_matrix(&m2, A_cols_B_rows_no, B_cols_no);
        
        fill_matrix_with_rand(&m1);
        fill_matrix_with_rand(&m2);

        char A_file[10];
        char B_file[10];

        snprintf(A_file, 10, "A%d.txt", i);
        snprintf(B_file, 10, "B%d.txt", i); 
        
        write_matrix_to_file(&m1, A_file);
        write_matrix_to_file(&m2, B_file);

        write_to_main_file(A_file, B_file, i);
    }

}

void test(char* main_file, char* file1, char* file2, char* exfile) {
    
    struct matrix* matrix1 = read_matrix_from(file1);
    struct matrix* matrix2 = read_matrix_from(file2);

    struct matrix* calculated = read_matrix_from(exfile);

    struct matrix* result = multiply(matrix1, matrix2);
    
    if (are_matrices_equal(calculated, result)) {
        printf("Matrices from file %s were succesfully multiplied.\n", main_file);
    } else {
        printf("Something went wrong in multiplication of matrices from file %s\n", main_file);
    }

    free_matrix(matrix1);
    free_matrix(matrix2);
    free_matrix(calculated);
    free_matrix(result);

    return;
}
