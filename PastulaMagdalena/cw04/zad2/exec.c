#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

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


int main(int argc, char** argv) {

    char* sig_hdl = argv[1];
    int sig_no = atoi(argv[2]);

    if (strcmp(sig_hdl,"pending")!=0) {
        raise(sig_no);
    }

    bool checked = check_signal_handling(sig_hdl, sig_no);

    printf("Exec testing:\n");
    if (checked) {
        printf("Handling: %s signal: %d - worked in exec.\n", sig_hdl, sig_no);
    } else {
        printf("Handling: %s signal: %d - did not work in exec.\n", sig_hdl, sig_no);    
    }

    return 0;
}