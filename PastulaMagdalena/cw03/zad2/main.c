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

void prepare_common_files(char* exfile) {
    int fp = open("comm.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    close(fp);
    //if (strcmp(mode, "comm")==0) {
    fp = open(exfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXO);
    close(fp);
    //}
}

int main(int argc, char** argv) {
    check_argc(argc);
    check_argv(argv);

    int arg_indx = 1;

    char* file_name = argv[arg_indx++];
    int children_no = atoi(argv[arg_indx++]);
    char* max_time = argv[arg_indx++];
    char* mode = argv[arg_indx];

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
        execlp("./child", "child", mfile1, mfile2, exfile, indx_ch, max_time, mode, NULL);
    } else {
        printf("Program 'main', pid: %d\n", (int)getpid());
        
        if (strcmp(mode, "sep")==0) {
            pid_t paste_pid = fork();

            if (paste_pid == 0) {
                char* ch_children_no = argv[2];
                
                execlp("./paste_sep", "paste_sep", exfile, ch_children_no, mfile1, mfile2, NULL);
            } else {
                int stat;
                waitpid(paste_pid ,&stat, 0);
            }
        } else {
            int stat;
            indx=0;

            while(indx < children_no) { 
                wait(&stat);
                //pid_t child_pid = wait(&stat);
                //printf("Child process with pid %d did %d multiplication(s).\n", child_pid, WEXITSTATUS(stat));
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
    }
}

void check_argv(char** argv) {
    char* mode = argv[4];

    if (strcmp(mode, "comm")!=0 && strcmp(mode, "sep")!=0) {
        printf("Not known mode. Exiting.\n");
        exit(-1);
    }
}
