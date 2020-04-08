#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

bool signal_handled = false;

void check_argc(int argc);
void check_argv(char** argv);

void handle_signal(int sig_no);

void set_signal_handling(char* sig_hdl, int sig_no);
bool check_signal_handling(char* sig_hdl, int sig_no);


int main(int argc, char** argv) {

    check_argc(argc);
    check_argv(argv);

    char* mode = argv[1];
    char* signal_handler = argv[2];

    for (int i=1;i<21;i++) {
        if (i!=9 && i!=19) {
            set_signal_handling(signal_handler, i);
        }
    }
    
    printf("Parent testing:\n");
    bool checked;

    for(int i=1;i<21;i++) {
        if (i!=9 && i!=19) {
            if (raise(i) != 0) {
                printf("Problem in raising signal %d in main. Exiting.\n", i);
                exit(-1);
            }

            checked = check_signal_handling(signal_handler, i);

            if (checked) {
                printf("Handling: %s signal: %d - worked in parent process.\n", signal_handler, i);
            } else {
                printf("Handling: %s signal: %d - did not work in parent process.\n", signal_handler, i);    
            }
        }
    }
    

    if (strcmp(mode, "fork")==0) {
        pid_t child_pid = fork();

        if (child_pid == 0) {
            printf("Fork testing:\n");
            
            for (int i=1;i<21;i++) {
                if (i!=9 && i!=19) {
                    if (strcmp(signal_handler, "pending") != 0) {
                        raise(i);
                    }

                    sleep(1);
                    
                    checked = check_signal_handling(signal_handler, i);

                    if (checked) {
                        printf("Handling: %s signal: %d - worked in fork.\n", signal_handler, i);
                    } else {
                        printf("Handling: %s signal: %d - did not work in fork.\n", signal_handler, i);    
                    }
                }

            }
            
        } else {
            int stat;
            wait(&stat);
        }

    } else {
        execlp("./exec", "exec", signal_handler, NULL);
    }

    return 0;
}

void check_argc(int argc) {
    if (argc == 1) {
        printf("No arguments. Exiting.\n");
        exit(-1);
    } else if(argc == 2) {
        printf("Not specified way of handling signal.Exiting.\n");
        exit(-1);
    }
}

void check_argv(char** argv) {
    
    char* mode = argv[1];
    char* sig_hdl = argv[2];

    if (strcmp(mode, "fork")!=0 && strcmp(mode, "exec")!=0) {
        printf("Wrong inheritance mode. Exiting.\n");
        exit(-1);
    }
    if (strcmp(sig_hdl, "ignore")!=0 && strcmp(sig_hdl, "handler")!=0 && strcmp(sig_hdl, "mask")!=0 && strcmp(sig_hdl, "pending")!=0) {
        printf("Wrong signal handling mode. Exiting.\n");
        exit(-1);
    }
    if (strcmp(mode, "exec")==0 && strcmp(sig_hdl, "handler")==0) {
        printf("Cannot test adding signal handler in exec mode. Exiting.\n");
        exit(-1);
    }
}

void handle_signal(int sig_no) {
    signal_handled = true;
    printf("Received signal no: %d.\n", sig_no);
}

void set_signal_handling(char* sig_hdl, int sig_no) {

    if (strcmp(sig_hdl, "ignore")==0) {
        signal(sig_no, SIG_IGN);
    } else if (strcmp(sig_hdl, "handler")==0){
        
        struct sigaction act;
        act.sa_handler = handle_signal;
        sigfillset(&act.sa_mask);
        sigaction(sig_no, &act, NULL);
    
    } else if(strcmp(sig_hdl, "mask")==0 || strcmp(sig_hdl, "pending")==0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, sig_no);
        sigprocmask(SIG_BLOCK, &set, NULL);
    }
}

bool check_signal_handling(char* sig_hdl, int sig_no) {
    sigset_t set;
    sigemptyset(&set);

    if (strcmp(sig_hdl, "ignore")==0) {
        
        sigpending(&set);

        if (sigismember(&set, sig_no) != 1 && !signal_handled) {
            return true;
        }
    } else if (strcmp(sig_hdl, "handler")==0){
        if (signal_handled) {
            signal_handled = false;
            return true;
        }
    } else if(strcmp(sig_hdl, "mask")==0) {
        
        sigprocmask(SIG_SETMASK, NULL, &set);

        if (sigismember(&set, sig_no) == 1) {
            sigprocmask(SIG_SETMASK, &set, NULL);
            return true;
        }

        sigprocmask(SIG_SETMASK, &set, NULL);

    } else if (strcmp(sig_hdl, "pending")==0) {
        
        sigpending(&set);

        if (sigismember(&set, sig_no) == 1) {
            return true;
        }
    }

    return false;
}
