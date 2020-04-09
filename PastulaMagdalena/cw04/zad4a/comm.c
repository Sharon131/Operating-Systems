
#include "comm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void register_sig_handler(int sig_no, void (*func)(int, siginfo_t *, void *)) {
    sigset_t set;
    sigfillset(&set);
    
    struct sigaction act;
    act.sa_sigaction = func;
    act.sa_flags = SA_SIGINFO;
    act.sa_mask = set;
    
    sigaction(sig_no, &act, NULL);
}


void send_sig(char* mode, int sig_no, int catcher_pid, int sigqueue_val) {
    if (strcmp(mode, "kill")==0) {
        kill(catcher_pid, sig_no);
    } else if (strcmp(mode, "sigqueue")==0) {
        
        union sigval value;
        value.sival_int = sigqueue_val;
        sigqueue(catcher_pid, sig_no, value);
        
    } else if (strcmp(mode, "sigrt")==0) {
        kill(catcher_pid, sig_no);
    }
}
