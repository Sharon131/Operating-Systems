#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

void check_argc(int argc);
void check_argv(char** argv);

void prepare_common_files(char* exfile);
void write_to_paste_file(char* file, int value);

void set_child_limits(int time_limit, float mem_limit);
void get_and_print_stats();


int main(int argc, char** argv) {
    check_argc(argc);
    check_argv(argv);

    int arg_indx = 1;

    char* file_name = argv[arg_indx++];
    int children_no = atoi(argv[arg_indx++]);
    char* max_time = argv[arg_indx++];
    char* mode = argv[arg_indx++];
    int time_limit = atoi(argv[arg_indx++]);
    float memory_limit = atof(argv[arg_indx]);

    // Read files names
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
    // Reding file names end
    
    //Preparing common file for children
    prepare_common_files(exfile);

    pid_t* children_pids = calloc(children_no, sizeof(pid_t));

    int indx = 0;
    children_pids[indx++] = fork();

    while(children_pids[indx-1]!= 0 && indx<children_no) {
        children_pids[indx] = fork();
        indx++;
    }


    if(children_pids[indx-1]==0) {
        char indx_ch[5];
        snprintf(indx_ch, 4, "%d", indx-1);
        set_child_limits(time_limit, memory_limit);
        execlp("./child", "child", mfile1, mfile2, exfile, indx_ch, max_time, mode, NULL);
    } else {
        printf("Program 'main', pid: %d\n", (int)getpid());
        
        if (strcmp(mode, "sep")==0) {
            char* paste_file = "sep_comm.txt";
            
            pid_t paste_pid = fork();

            if (paste_pid == 0) {
                char* ch_children_no = argv[2];
                execlp("./paste_sep", "paste_sep", exfile, ch_children_no, mfile1, mfile2, NULL);
            } else {
                write_to_paste_file(paste_file, children_no);

                int stat;
                int index = 0;

                while (index < children_no) {
                    pid_t child_pid = wait(&stat);
                    printf("Stats for child process %d:\n", child_pid);
                    get_and_print_stats();
                    index++;
                }

                write_to_paste_file(paste_file, 0);
                pid_t child_pid = wait(&stat);
                printf("Paste process with pid %d stats:\n", child_pid);
                get_and_print_stats();
            }
        } else {
            int stat;
            indx=0;

            while(indx < children_no) { 
                pid_t child_pid = wait(&stat);
                //pid_t child_pid = wait(&stat);
                //printf("Child process with pid %d did %d multiplication(s).\n", child_pid, WEXITSTATUS(stat));
                printf("Stats for child process %d:\n", child_pid);
                get_and_print_stats();
                indx++;
            }
        }
        
    }

    return 0;
}


void check_argc(int argc) {
    if (argc == 1) {
        printf("Program executed with no arguments.\n");
        exit(-1);
    } else if (argc == 2) {
        printf("Number of child processes was not given.\n");
        exit(-1);
    } else if (argc == 3) {
        printf("Time limit for child process was not given.\n");
        exit(-1);
    } else if (argc == 4) {
        printf("Creating result file mode not specified.\n");
        exit(-1);
    } else if (argc == 5) {
        printf("Time limit not specified.\n");
        exit(-1);
    } else if( argc == 6) {
        printf("Memory limit not specified.\n");
        exit(-1);
    }
}

void check_argv(char** argv) {
    char* mode = argv[4];

    if (strcmp(mode, "comm")!=0 && strcmp(mode, "sep")!=0) {
        printf("Not known mode. Exiting.\n");
        exit(-1);
    }
}

void prepare_common_files(char* exfile) {
    int fp = open("comm.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    close(fp);
    fp = open(exfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    close(fp);
}

void write_to_paste_file(char* file, int value) {
    int fp_paste = open(file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    char buff[10];
    snprintf(buff, 10, "%d\n", value);
    write(fp_paste, buff, strlen(buff));
    close(fp_paste);
    
}

void set_child_limits(int time_limit, float mem_limit) {
    struct rlimit memory;
    memory.rlim_cur = (rlim_t)(mem_limit*1024*1024);
    memory.rlim_max = mem_limit*1024*1024;
    if (setrlimit(RLIMIT_AS, &memory)!= 0) {
        printf("Unable to set this memory limit.\n");
        exit(-1);
    }

    struct rlimit time;
    time.rlim_cur = time_limit;
    time.rlim_max = time_limit;
    if (setrlimit(RLIMIT_CPU, &time) != 0) {
        printf("Unable to set this time limit.\n");
        exit(-1);
    }
}

void get_and_print_stats() {
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);

    printf("User time: %ld\n", usage.ru_utime.tv_usec);
    printf("System time: %ld\n", usage.ru_stime.tv_usec);
    printf("Integral shared memory size: %ld\n", usage.ru_ixrss);
    printf("Integral unshared data size: %ld\n", usage.ru_idrss);
    printf("Integral unshared stack size: %ld\n", usage.ru_isrss);
    printf("---------------------------------\n");
}
