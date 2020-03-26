#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void check_argc(int argc) {
    if (argc == 1) {
        printf("Program executed with no arguments.\n");
        exit(-1);
    } else if (argc == 2) {
        printf("Number of child processes was not given.\n");
        exit(-1);
    }
}


int main(int argc, char** argv) {
    check_argc(argc);
    
    int arg_indx = 1;

    char* file_name = argv[arg_indx++];
    int children_no = atoi(argv[arg_indx]);

    pid_t* children_pids = calloc(children_no, sizeof(pid_t));

    int indx = 0;
    children_pids[indx++] = fork();

    while(children_pids[indx-1]!= 0 && indx<children_no) {
        //if (children_pids==NULL || children_pids[indx-1] == 0) {
        //    printf("Child process exiting while loop.\n");
        //    break;
        //}
        //else{
        children_pids[indx] = fork();
        //}
        indx++;
    }

    if(children_pids[indx-1]==0) {
        execvp("./child", NULL);
    } else {
        printf("Program 'main', pid: %d\n", (int)getpid());
    }

    return 0;
}

