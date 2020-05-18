#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>

#define PIXELS_VALS_NO  256

#define SIGN_ARGS_NO    3
#define BLOCK_ARGS_NO   3
#define INTER_ARGS_NO   3

int image_rows_no;
int image_cols_no;

int** image;
int** histogram;

int threads_no;
pthread_t* threads_ptrs;


void check_args (int argc, char** argv);

void get_image_from(char* file_name);
int read_next_num_from(int fd);

void make_args_for(char* mode, int thread_no, int threads_no, int** threads_args);

void* sign_thread_func(void* args);
void* block_thread_func(void* args);
void* interleaved_thread_func(void* args);

void subtract_times(struct timespec* x, struct timespec* y, struct timespec* res);
void save_result_to(char* file_name, int threads_no);

void save_times_to_file(struct timespec* start_time, char* mode) {

    int fd = open("times.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    char buff[100];
    
    snprintf(buff, 99, "-----------------------------------------------\n");
    write(fd, buff, strlen(buff));

    snprintf(buff, 99, "Times for %s mode with %d thread(s).\n", mode, threads_no);
    write(fd, buff, strlen(buff));

    for (int i=0;i<threads_no;i++) {
        struct timespec* stat;
        pthread_join(threads_ptrs[i], (void**)&stat);
        if (stat != NULL) {
            printf("Thread %d ended. Time taken: %ld s %ld ms %ld us.\n", i, stat->tv_sec, stat->tv_nsec/1000000, (stat->tv_nsec%1000000)/1000);
            snprintf(buff, 99, "Time taken for thread %d: %ld s %ld ms %ld us.\n", i, stat->tv_sec, stat->tv_nsec/1000000, (stat->tv_nsec%1000000)/1000);
            write(fd, buff, strlen(buff));    
        }
    }

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    struct timespec time_taken;
    subtract_times(&end_time, start_time, &time_taken);

    printf("General time taken: %ld s %ld ms %ld us\n", time_taken.tv_sec, time_taken.tv_nsec/1000000, (time_taken.tv_nsec%1000000)/1000);
    snprintf(buff, 99, "General time taken: %ld s %ld ms %ld us\n", time_taken.tv_sec, time_taken.tv_nsec/1000000, (time_taken.tv_nsec%1000000)/1000);
    write(fd, buff, strlen(buff));

    close(fd);
}

void subtract_times(struct timespec* x, struct timespec* y, struct timespec* res) {
    
    if (y->tv_nsec > x->tv_nsec) {
        x->tv_nsec += 1000000000;
        x->tv_sec -= 1;
    }

    res->tv_sec = x->tv_sec - y->tv_sec;
    res->tv_nsec = x->tv_nsec - y->tv_nsec;
}

void at_exit() {
    
    for (int i=0;i<image_rows_no;i++) {
        free(image[i]);
    }

    for (int i=0;i< threads_no;i++) {
        free(histogram[i]);
    }

    free(image);
    free(histogram);
    free(threads_ptrs);
}

/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    check_args(argc, argv);

    threads_no = atoi(argv[1]);
    char* mode = argv[2];
    char* file_in = argv[3];
    char* file_out = argv[4];

    get_image_from(file_in);    

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    histogram = calloc(threads_no, sizeof(int*));
    threads_ptrs = calloc(threads_no, sizeof(pthread_t));
    int** threads_args = calloc(threads_no, sizeof(int*));
    
    for (int i=0;i<threads_no;i++) {
        histogram[i] = calloc(PIXELS_VALS_NO, sizeof(int));

        if (strcmp(mode, "sign") == 0) {
            
            make_args_for(mode, i, threads_no, threads_args);
            pthread_create(&threads_ptrs[i], NULL, sign_thread_func, (void*)threads_args[i]);
        
        } else if (strcmp(mode, "block") == 0) {
            
            make_args_for(mode, i, threads_no, threads_args);
            pthread_create(&threads_ptrs[i], NULL, block_thread_func, (void*)threads_args[i]);

        } else {
            
            make_args_for(mode, i, threads_no, threads_args);
            pthread_create(&threads_ptrs[i], NULL, interleaved_thread_func, (void*)threads_args[i]);
        }
    
    }

    save_times_to_file(&tp, mode);

    // add sigthreads mask to handle ctr + c

    save_result_to(file_out, threads_no);

    at_exit();

    return 0;
}


/*--------------------------------------------------------------*/


void check_args (int argc, char** argv) {
    if (argc < 5) {
        printf("Not enough arguments. Exiting.\n");
        exit(-1);
    }

    if (atoi(argv[1]) <= 0) {
        printf("Number of threads must be greater that zero. Exiting.\n");
        exit(-1);
    }

    if (strcmp(argv[2], "sign")!= 0 && strcmp(argv[2], "block")!=0 && strcmp(argv[2], "interleaved")!=0) {
        printf("Mode %s is not known. Exiting.\n", argv[2]);
        exit(-1);
    }

    if (strcmp(argv[3] + strlen(argv[3]) - 4,".pgm") != 0) {
        printf("Extension %s is not correct. It should be .pgm. Exiting.\n", argv[3]);
        exit(-1);
    }

    if (strcmp(argv[4] + strlen(argv[4]) - 4, ".txt") != 0) {
        printf("Output file should have extension .txt not %s. Exiting\n", argv[4]);
        exit(-1);
    }
}


void get_image_from(char* file_name) {
    int fd = open(file_name, O_RDONLY);

    if (fd <0) {
        printf("Error while opening input file: %s. Exiting.\n", file_name);
        exit(-1);
    }

    read_next_num_from(fd);

    image_cols_no = read_next_num_from(fd);
    image_rows_no = read_next_num_from(fd);

    read_next_num_from(fd);

    image = calloc(image_rows_no, sizeof(int*));

    for (int i=0;i<image_rows_no;i++) {
        image[i] = calloc(image_cols_no, sizeof(int));

        for(int j=0;j<image_cols_no;j++) {
            image[i][j] = read_next_num_from(fd);
        }
    }

    close(fd);
}


int read_next_num_from(int fd) {
    char read_char;

    int code = read(fd, &read_char, 1);

    while ((read_char < 48 || read_char > 57) && code != 0){
        if (read_char == '#'){
            while (read_char != 10 && read_char != 13 && code != 0){
                code = read(fd, &read_char, 1);
            }
        }
        
        code = read(fd, &read_char, 1);
    }

    if (code <= 0)  return -1;
    
    int num_to_return = 0;
    while (read_char <= 57 && read_char >= 48 && code != 0) {
        num_to_return = num_to_return*10 + ((int)read_char-48);
        code = read(fd, &read_char, 1);
    }

    if (code <= 0)  return -1;

    return num_to_return;
}


void make_args_for(char* mode, int thread_no, int threads_no, int** threads_args) {
    
    if (strcmp(mode, "sign") == 0) {
        threads_args[thread_no] = calloc(SIGN_ARGS_NO, sizeof(int));
            
        threads_args[thread_no][0] = thread_no;
        threads_args[thread_no][1] = thread_no*(PIXELS_VALS_NO/threads_no);
        threads_args[thread_no][2] = (thread_no+1)*(PIXELS_VALS_NO/threads_no);
        if (thread_no == threads_no-1) {
            threads_args[thread_no][2] += PIXELS_VALS_NO%threads_no;
        }

    } else if(strcmp(mode, "block") == 0) {
        threads_args[thread_no] = calloc(BLOCK_ARGS_NO, sizeof(int));
        
        threads_args[thread_no][0] = thread_no;
        threads_args[thread_no][1] = thread_no*(image_cols_no/threads_no);
        threads_args[thread_no][2] = (thread_no+1)*(image_cols_no/threads_no);

        if (thread_no == threads_no-1) {
            threads_args[thread_no][2] += image_cols_no%threads_no;
        }

    } else {
        threads_args[thread_no] = calloc(INTER_ARGS_NO, sizeof(int));
        
        threads_args[thread_no][0] = thread_no;
        threads_args[thread_no][1] = threads_no;
        threads_args[thread_no][2] = thread_no;
    }
}



void* sign_thread_func(void* args) {
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    int thread_no = ((int*)args)[0];
    int start_pixel_val = ((int*)args)[1];
    int stop_pixel_val = ((int*)args)[2];
    
    for (int i=0;i<image_rows_no;i++) {
        for (int j=0;j<image_cols_no;j++) {

            if (image[i][j] >= start_pixel_val && image[i][j] < stop_pixel_val) {
                histogram[thread_no][image[i][j]]++;
            }
        }
    }

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    struct timespec* time_taken = calloc(1,  sizeof(struct timespec));
    subtract_times(&end_time, &start_time, time_taken);

    return (void*)time_taken;
}


void* block_thread_func(void* args) {
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    int thread_no = ((int*)args)[0];
    int start_x = ((int*)args)[1];
    int stop_x = ((int*)args)[2];
    
    for (int i=0;i<image_rows_no;i++) {
        for (int j=start_x;j<stop_x;j++) {
            histogram[thread_no][image[i][j]]++;
        }
    }

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    struct timespec* time_taken = calloc(1,  sizeof(struct timespec));
    subtract_times(&end_time, &start_time, time_taken);

    return (void*)time_taken;

}


void* interleaved_thread_func(void* args) {
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    int thread_no = ((int*)args)[0];
    int step = ((int*)args)[1];
    int start_x = ((int*)args)[2];

    for (int i=0;i<image_rows_no;i++) {

        for (int j=start_x;j<image_cols_no;j+=step) {
            histogram[thread_no][image[i][j]]++;
        }
    }

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    struct timespec* time_taken = calloc(1,  sizeof(struct timespec));
    subtract_times(&end_time, &start_time, time_taken);

    return (void*)time_taken;
}


void save_result_to(char* file_name, int threads_no) {
    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    char buff[100];    
    for (int i=0;i<PIXELS_VALS_NO;i++) {
        int sum = 0;

        for (int j=0;j<threads_no;j++) {
            sum += histogram[j][i];
        }
        
        snprintf(buff, 99,"%d: %d\n", i, sum);
        write(fd, buff, strlen(buff));
    }

    close(fd);
}