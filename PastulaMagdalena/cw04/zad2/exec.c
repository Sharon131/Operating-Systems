#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

bool check_signal_handling(char* sig_hdl, int sig_no);


int main(int argc, char** argv) {

    char* sig_hdl = argv[1];

    printf("Exec testing:\n");

    for (int i=1;i<21;i++) {
        if (i!=9 && i!=19) {
            if (strcmp(sig_hdl,"pending")!=0) {
                raise(i);
            }

            sleep(1);
            
            bool checked = check_signal_handling(sig_hdl, i);

            if (checked) {
                printf("Handling: %s signal: %d - worked in exec.\n", sig_hdl, i);
            } else {
                printf("Handling: %s signal: %d - did not work in exec.\n", sig_hdl, i);    
            }
        }
    }
    
    return 0;
}

bool check_signal_handling(char* sig_hdl, int sig_no) {
    sigset_t set;
    sigemptyset(&set);

    if (strcmp(sig_hdl, "ignore")==0) {
        
        sigpending(&set);

        if (sigismember(&set, sig_no) != 1) {
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
