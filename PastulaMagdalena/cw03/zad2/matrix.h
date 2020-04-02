#ifndef MATRICES_
#define MATRICES_

#include <stdbool.h>

struct matrix {
    int** vals;
    int rows_no;
    int cols_no;
};

void print_matrix(struct matrix* mx);
int get_num_from(char* string, int* result);
struct matrix* read_matrix_from(char* file_name);

void create_matrix(struct matrix* m, int rows_no, int cols_no);
bool are_matrices_equal(struct matrix* m1, struct matrix* m2);
int multiply_one_row(int** m1, int** m2, int row, int col, int size);
struct matrix* multiply(struct matrix* m1, struct matrix* m2);

struct matrix* get_product_col(struct matrix* m1, struct matrix* m2, int col_no);

#endif