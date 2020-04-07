#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>

void print_matrix(struct matrix* mx) {
    printf("Printing matrix:\n");
    for (int i=0;i<mx->rows_no;i++) {
        printf("%d\t", i);
        for (int j=0;j<mx->cols_no;j++) {
            printf("%d ", mx->vals[i][j]);    
        }
        printf("\n");
    }
}

int get_num_from(char* string, int* result) {
    *result = 0;
    int indx = 0;

    while ((unsigned int)string[indx]<='9' && (unsigned int)string[indx]>='0') {
        *result *= 10;
        *result += string[indx]-'0';
        indx++;
    }

    return indx+1;
}

struct matrix* read_matrix_from(char* file_name) {
    FILE* fp = fopen(file_name, "r");

    char* buff = NULL;
    size_t buff_size = 0;
    getline(&buff,&buff_size,fp);

    int rows_no = 0;
    int cols_no = 0;

    int indx = get_num_from(buff, &rows_no);
    get_num_from(buff+indx, &cols_no);
    
    int** array = calloc(rows_no,sizeof(int*));

    for(int i=0;i<rows_no;i++) {
        array[i] = calloc(cols_no, sizeof(int));
        getline(&buff,&buff_size,fp);
        int index = 0;
        for(int j=0;j<cols_no;j++) {
            index += get_num_from(buff+index, array[i]+j);
        }
        
    }

    fclose(fp);

    struct matrix* result= calloc(1, sizeof(struct matrix));
    result->vals = array;
    result->rows_no = rows_no;
    result->cols_no = cols_no;
    return result;
}

void create_matrix(struct matrix* m, int rows_no, int cols_no) {
    m->rows_no = rows_no;
    m->cols_no = cols_no;
    m->vals = calloc(rows_no, sizeof(int*));

    for (int i=0;i<rows_no;i++) {
        m->vals[i] = calloc(cols_no, sizeof(int)); 
    }
}


bool are_matrices_equal(struct matrix* m1, struct matrix* m2) {
    if (m1->rows_no != m2->rows_no || m1->cols_no != m2->cols_no) {
        return false;
    }

    for (int i=0;i<m1->rows_no;i++) {
        for (int j=0;j<m1->cols_no;j++) {
            if (m1->vals[i][j] != m2->vals[i][j]) {
                return false;
            }
        }
    }

    return true;
}

int multiply_one_row(int** m1, int** m2, int row, int col, int size) {
    int res = 0;

    for (int i=0;i<size;i++) {
        res += m1[row][i]*m2[i][col]; 
    }

    return res;
}

struct matrix* multiply(struct matrix* m1, struct matrix* m2) {

    struct matrix* result = calloc(1, sizeof(struct matrix));
    result->rows_no = m1->rows_no;
    result->cols_no = m2->cols_no;
    result->vals = calloc(result->rows_no, sizeof(int*));

    for (int i=0;i<result->rows_no;i++) {
        result->vals[i] = calloc(result->cols_no, sizeof(int));
        for (int j=0;j<result->cols_no;j++) {
            result->vals[i][j] = multiply_one_row(m1->vals, m2->vals, i, j, m1->cols_no);
        }
    }

    return result;
}


struct matrix* get_product_col(struct matrix* m1, struct matrix* m2, int col_no) {
    struct matrix* result = calloc(1, sizeof(struct matrix));

    create_matrix(result, m1->rows_no, 1);

    for (int i=0;i<m1->rows_no;i++) {
        result->vals[i][0] = multiply_one_row(m1->vals, m2->vals, i, col_no, m1->cols_no);
    }

    return result;
}

void free_matrix(struct matrix* m) {
    if (m != NULL) {
        if (m->vals != NULL) {
            for (int i=0;i<m->rows_no;i++) {
                if (m->vals[i] != NULL) {
                    free(m->vals[i]);
                }
            }
            free(m->vals);
        }
        free(m);
    }
}