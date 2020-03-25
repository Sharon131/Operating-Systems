#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>

int execute_command(char* command, char** argv);

char rand_char();
void generate(char* file_name, int records_no, int record_len);

void sort_records(char** records, int records_no, int record_len);

void sort_lib(char* file_name, int records_no, int record_len);
void copy_lib(char* file_name1, char* file_name2, int records_no, int record_len);
void sort_sys(char* file_name, int records_no, int record_len);
void copy_sys(char* file_name1, char* file_name2, int records_no, int record_len);

void printfTimes(struct tms* su_time_start, struct tms* su_time_end) {
	printf("User time: %f s\n", ((double)su_time_end->tms_utime-su_time_start->tms_utime)/sysconf(_SC_CLK_TCK));
	printf("System time: %f s\n", ((double)su_time_end->tms_stime-su_time_start->tms_stime)/sysconf(_SC_CLK_TCK));
}

void printfCommand(char* command, char** argv) {
    if (strcmp(command, "sort")==0) {
        int records_no = atoi(argv[1]);
        int record_len = atoi(argv[2]);
        
        printf("Times taken for command %s for file %s with args %d %d in mode %s\n", command, argv[0], records_no, record_len, argv[3]);
    } else if (strcmp(command, "copy")==0) {
        char* fn1 = argv[0];
        char* fn2 = argv[1];
        int records_no = atoi(argv[2]);
        int record_len = atoi(argv[3]);
        
        printf("Times taken for command %s for files %s %s with args %d %d in mode %s\n", command, fn1, fn2, records_no, record_len, argv[4]);
    }
}

int main(int argc, char** argv) {
    
    int arg_index = 1;
    struct tms sys_us_time_start;
    struct tms sys_us_time_end;
    
    while (arg_index < argc) {
        char* command = argv[arg_index];
        arg_index++;

        printfCommand(command, argv+arg_index);
		
        times(&sys_us_time_start);
		arg_index += execute_command(command, argv+arg_index);
        times(&sys_us_time_end);

        if (strcmp(command, "sort")==0 || strcmp(command, "copy")==0) {
            printfTimes(&sys_us_time_start, &sys_us_time_end);
        }
    }

    return 0;
}


int execute_command(char* command, char** argv) {
    if (strcmp(command, "generate")==0) {
        char* file_name = argv[0];
        int records_no = atoi(argv[1]);
        int record_len = atoi(argv[2])+1;

        generate(file_name, records_no, record_len);

        return 3;
    } else if (strcmp(command, "sort")==0) {
        char* file_name = argv[0];
        int records_no = atoi(argv[1]);
        int record_len = atoi(argv[2])+1;
        char* mode = argv[3];
        
        if (strcmp(mode, "sys")==0) {
            sort_sys(file_name, records_no, record_len);
        } else if (strcmp(mode, "lib")==0) {
            sort_lib(file_name, records_no, record_len);
        } else {
            printf("Not known mode.\n");
        }
        
        return 4;
    } else if (strcmp(command, "copy")==0) {
        char* fn1 = argv[0];
        char* fn2 = argv[1];
        int records_no = atoi(argv[2]);
        int record_len = atoi(argv[3])+1;
        char* mode = argv[4];

        if (strcmp(mode, "sys")==0) {
            copy_sys(fn1, fn2, records_no, record_len);
        } else if (strcmp(mode, "lib")==0) {
            copy_lib(fn1, fn2, records_no, record_len);            
        } else {
            printf("Not known mode.\n");
        }
        
        return 5;
    } else {
        printf("Not known command.\n");
        exit(-1);
    }
}

char rand_char() {
    return (char)((rand()%95)+33); 
}

void generate(char* file_name, int records_no, int record_len) {
    srand((unsigned int)time(NULL));

    int fp = creat(file_name, S_IRWXU | S_IRWXG | S_IRWXO);

    int record_indx = 0;
    char* to_write = calloc(record_len, sizeof(char));
        
    while (record_indx < records_no) {
        for (int i=0;i<record_len-1;i++) {
            to_write[i] = rand_char();
        }
        to_write[record_len-1] = '\n';
        write(fp, to_write, record_len);
        record_indx++;
    }

    free(to_write);

    close(fp);
}

bool isFirstLesser(char* rec1, char* rec2, int rec_len) {

    for (int i=0;i<rec_len;i++) {
        if ((unsigned int)rec1[i] < (unsigned int)rec2[i]) {
            return true;
        } else if ((unsigned int)rec1[i] > (unsigned int)rec2[i]) {
            return false;
        }
    }
    return false;
}

int partition_lib(FILE* fp, int l, int r, int rec_len){
    
    fseek (fp, l*rec_len, 0);
    char* pivot_char = calloc(rec_len, sizeof(char));
    fread(pivot_char, sizeof(char), rec_len, fp);
    char* record_j = calloc(rec_len, sizeof(char));
    char* record_i = calloc(rec_len, sizeof(char));

    int i=l+1;
    for (int j=l+1;j<=r;j++) {
        fseek(fp, j*rec_len, 0);
        fread(record_j, sizeof(char), rec_len, fp);

        if (isFirstLesser(record_j, pivot_char, rec_len)) {
            fseek(fp, i*rec_len, 0);
            fread(record_i, sizeof(char), rec_len, fp);
            fseek(fp, i*rec_len, 0);
            fwrite(record_j, sizeof(char), rec_len, fp);
            fseek(fp, j*rec_len, 0);
            fwrite(record_i, sizeof(char), rec_len, fp);
            i++;
        }
    }

    fseek(fp, (i-1)*rec_len, 0);
    fread(record_i, sizeof(char), rec_len, fp);
    fseek(fp, (i-1)*rec_len, 0);
    fwrite(pivot_char, sizeof(char), rec_len, fp);
    fseek(fp, l*rec_len, 0);
    fwrite(record_i, sizeof(char), rec_len, fp);

    free(pivot_char);
    free(record_j);
    free(record_i);
    
    return i-1;
}

void quick_sort_lib(FILE* fp, int l, int r, int rec_len) {

    if (l < r){
        int pi = partition_lib(fp, l, r, rec_len);
        quick_sort_lib(fp, l, pi-1, rec_len);
        quick_sort_lib(fp, pi+1, r, rec_len);
    }
}

void sort_lib(char* file_name, int records_no, int record_len) {
    FILE* fp = fopen(file_name, "r+");

    quick_sort_lib(fp, 0, records_no-1, record_len);

    fclose(fp);
}

void copy_lib(char* file_name1, char* file_name2, int records_no, int record_len) {
    FILE* fp1 = fopen(file_name1, "r+");
    FILE* fp2 = fopen(file_name2, "w");
    
    char read_c;

    while (fread(&read_c, sizeof(char), 1, fp1) > 0) {
        fwrite(&read_c, sizeof(char), 1, fp2);
    }

    printf("\n");
    fclose(fp1);
    fclose(fp2);
}

int partition_sys(int fp, int l, int r, int rec_len){
    lseek (fp, l*rec_len, SEEK_SET);
    char* pivot_char = calloc(rec_len, sizeof(char));
    read(fp, pivot_char, rec_len);
    char* record_j = calloc(rec_len, sizeof(char));
    char* record_i = calloc(rec_len, sizeof(char));

    int i=l+1;
    for (int j=l+1;j<=r;j++) {
        lseek(fp, j*rec_len, SEEK_SET);
        read(fp, record_j, rec_len);

        if (isFirstLesser(record_j, pivot_char, rec_len)) {
            lseek(fp, i*rec_len, SEEK_SET);
            read(fp, record_i, rec_len);
            lseek(fp, i*rec_len, SEEK_SET);
            write(fp, record_j, rec_len);
            lseek(fp, j*rec_len, SEEK_SET);
            write(fp, record_i, rec_len);
            i++;
        }
    }

    lseek(fp, (i-1)*rec_len, SEEK_SET);
    read(fp, record_i, rec_len);
    lseek(fp, (i-1)*rec_len, SEEK_SET);
    write(fp, pivot_char, rec_len);
    lseek(fp, l*rec_len, SEEK_SET);
    write(fp, record_i, rec_len);

    free(pivot_char);
    free(record_j);
    free(record_i);
    
    return i-1;
}

void quick_sort_sys(int fp, int l, int r, int rec_len) {

    if (l < r){
        int pi = partition_sys(fp, l, r, rec_len);
        quick_sort_sys(fp, l, pi-1, rec_len);
        quick_sort_sys(fp, pi+1, r, rec_len);
    }
}


void sort_sys(char* file_name, int records_no, int record_len) {
    int fp = open(file_name, O_RDWR);

    quick_sort_sys(fp, 0, records_no-1, record_len);

    close(fp);
}

void copy_sys(char* file_name1, char* file_name2, int records_no, int record_len) {
    int fp1 = open(file_name1, O_RDONLY);
    int fp2 = creat(file_name2, S_IRWXU | S_IRWXG | S_IRWXO);

    char read_c;
    while (read(fp1, &read_c, 1) > 0) {
        write(fp2, &read_c, 1);
    }

    close(fp1);
    close(fp2);
}
