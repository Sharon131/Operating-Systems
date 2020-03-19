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

    char* to_write = calloc(records_no*record_len+1, sizeof(char));
    int record_indx = 0;
    int offset = 0;

    while (record_indx < records_no) {
        
        for (int i=0;i<record_len-1;i++) {
            to_write[i+offset] = rand_char();
        }
        to_write[offset+record_len-1] = '\n';
        offset += record_len;
        record_indx++;
    }

    write(fp, to_write, records_no*record_len);

    close(fp);
}

bool isFirstLesser(char* rec1, char* rec2, int rec_len) {

    for (int i=0;i<rec_len-1;i++) {
        if ((unsigned int)rec1[i] > (unsigned int)rec2[i]) {
            return false;
        }
    }
    return true;
}

int partition(char** records, int l, int r, int rec_len) {
    int pivot = l;

    int i = l;
    for (int j=l+1;j<=r;j++) {
        printf("i: %d, j: %d\n", i, j);
        if (isFirstLesser(records[j], records[pivot], rec_len)) {
            i++;
            char* tmp = records[j];
            records[j] = records[i];
            records[i] = tmp;
        }
    }
    char* tmp = records[i+1];
    records[i+1] = records[pivot];
    records[pivot] = tmp;
    return i+1;
}

void quick_sort(char** records, int l, int r, int record_len) {
    
    if (l < r){
        int pi = partition(records, l, r, record_len);
        quick_sort(records, l, pi-1, record_len);
        quick_sort(records, pi+1, r, record_len);
    }
}

void sort_records(char** records, int records_no, int record_len) {
    quick_sort(records, 0, records_no-1, record_len);
}

void sort_lib(char* file_name, int records_no, int record_len) {
    FILE* fp = fopen(file_name, "w+");

    char** records = calloc(records_no, sizeof(char*));

    for (int i=0;i<records_no;i++) {
        records[i] = calloc(record_len, sizeof(char));
        fread(records[i], record_len, record_len, fp);
        records[i][record_len-1] = 0;
        printf("Record %d: %s", i, records[i]);
    }

    sort_records(records, records_no, record_len);

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

void sort_sys(char* file_name, int records_no, int record_len) {
    int fp = open(file_name, O_RDWR);

    char** records = calloc(records_no, sizeof(char*));

    for (int i=0;i<records_no;i++) {
        records[i] = calloc(record_len, sizeof(char));
        read(fp, records[i], record_len);
        records[i][record_len-1] = 0;
        printf("Record %d: %s\n", i, records[i]);
    }
    
    sort_records(records, records_no, record_len);
    
    lseek(fp, 0, SEEK_SET);
    
    for (int i=0;i<records_no;i++) {
        records[i][record_len-1] = '\n';
        write(fp, records[i], record_len);
    } 

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
