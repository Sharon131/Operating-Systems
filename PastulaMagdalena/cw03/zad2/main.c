#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void check_argc(int argc);


int main(int argc, char** argv) {
    check_argc(argc);
    
    int arg_indx = 1;

    char* file_name = argv[arg_indx++];
    int children_no = atoi(argv[arg_indx++]);
    int max_time = atoi(argv[arg_indx++]);
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
    
    pid_t* children_pids = calloc(children_no, sizeof(pid_t));

    int indx = 0;
    children_pids[indx++] = fork();

    while(children_pids[indx-1]!= 0 && indx<children_no) {
        children_pids[indx] = fork();
        indx++;
    }

    if(children_pids[indx-1]==0) {
        execlp("./child", "child", mfile1, mfile2, exfile, mode, NULL);
    } else {
        printf("Program 'main', pid: %d\n", (int)getpid());
        int stat;
        wait(&stat);
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
