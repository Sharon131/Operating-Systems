#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_ARGS_NO     10
#define MAX_WORD_LEN    50


int get_commmands_count(char* file_name);
char** get_cmd_with_args(char* file_name, int indx);


/*----------------------------------------------------------*/


int main(int argc, char** argv) {

    char* file_name = argv[1];

    int commands_no = get_commmands_count(file_name);

    pid_t* children_pids = calloc(commands_no, sizeof(pid_t));

    int indx = 0;
    
    int prev_pipe[2];
    int curr_pipe[2];

    while (indx < commands_no) {

        pipe(curr_pipe);

        if (fork() == 0) {
            
            char** command = get_cmd_with_args(file_name, indx);

            if (indx != 0) {
                close(prev_pipe[1]);
                if (dup2(prev_pipe[0], STDIN_FILENO) == -1 ) {
                    printf("Problem in setting std input.Exiting.\n");
                    exit(1);
                }

            }

            if (indx != commands_no-1) {
                close(curr_pipe[0]);
                
                if (dup2(curr_pipe[1], STDOUT_FILENO) == -1) {
                    printf("Problem in setting std output.Exiting.\n");
                    exit(1);
                }
            }            

            execvp(command[0], command);
        }

        if (indx != 0) {
            close(prev_pipe[0]);
            close(prev_pipe[1]);
        }
        if (indx != commands_no-1) {
            prev_pipe[0] = curr_pipe[0];
            prev_pipe[1] = curr_pipe[1];
        }
        if (indx == commands_no-1) {
            close(curr_pipe[0]);
            close(curr_pipe[1]);
        }

        indx++;
    }

    
    free(children_pids);

    return 0;
}


/*----------------------------------------------------------*/


int get_commmands_count(char* file_name) {
    int fd = open(file_name, O_RDONLY);
    int indx = 0;

    char buff;

    while(read(fd, &buff, 1) != 0) {
        if (buff == '|') { 
            indx++;
        }
    }

    close(fd);

    return indx+1;
}

char** get_cmd_with_args(char* file_name, int indx) {
    char** command = calloc(MAX_ARGS_NO+2, sizeof(char*));
    int fd = open(file_name, O_RDONLY);

    int file_indx = 0;

    char buff;
    int stat = 10;
    while (file_indx < indx) {    
        if (read(fd, &buff, 1) <= 0) {
            printf("Not enough commands in file.\n");
            return NULL;
        }
        if (buff == '|') {
            file_indx++;
            stat = read(fd, &buff, 1);
        }
    }

    int i;
    for (i=0;i<MAX_ARGS_NO+1 && buff!='|' && stat>0;i++) {
        command[i] = calloc(MAX_WORD_LEN, sizeof(char));
        
        int char_index = 0;
        stat = read(fd, &buff, 1);
        if (buff == ' ' || buff == '\t') {
            stat = read(fd, &buff, 1);
        }

        if (buff == '|') {
            break;
        }

        while (buff != ' ' && buff != '\t' && buff!='|' && stat>0) {
            command[i][char_index] = buff;
            char_index++;
            stat = read(fd, &buff, 1);
        }
    }    

    close(fd);

    command[i] = NULL;

    return command;
}
