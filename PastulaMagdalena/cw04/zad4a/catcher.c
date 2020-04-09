#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "comm.h"

pid_t sender_pid;

int sigusr1_no = 0;

bool sigusr2_received = false;


void handle_sigusr1(int sig_no, siginfo_t* info, void* ucontext) {
    //printf("Caught SIGUSR1 signal in catcher.\n");
    sender_pid = info->si_pid;
    sigusr1_no++;
}

void handle_sigusr2(int sig_no, siginfo_t* info, void* ucontext) {
    //printf("Caught SIGUSR2 signal in catcher.\n");
    sender_pid = info->si_pid;
    sigusr2_received = true;
}


int main(int argc, char** argv) {

    printf("Catcher pid: %d\n", getpid());

    char* mode = argv[1];
    int sig_no1 = SIGUSR1;
    int sig_no2 = SIGUSR2;

    if (strcmp(mode, "sigrt")==0) {
        sig_no1 = SIGRTMIN+1;
        sig_no2 = SIGRTMIN+2;
    }

    register_sig_handler(sig_no1, handle_sigusr1);
    register_sig_handler(sig_no2, handle_sigusr2);
    
    while (!sigusr2_received) {
        ;
    }

    //send back
    for (int i=0;i<sigusr1_no;i++) {
        send_sig(mode, sig_no1, sender_pid, i);        
    }

    send_sig(mode, sig_no2, sender_pid, sigusr1_no);

    printf("Catcher caught %d SIGUSR1 signals.\n", sigusr1_no);

    return 0;
}
