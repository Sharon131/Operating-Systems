#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

#include "comm.h"

bool sigusr2_received = false;

int sigusr1s_no = 0;

int sigusr1_catcher_no = 0;
int sigusr1s_received_by_catcher = 0;


void handle_sigusr1(int sig_no, siginfo_t* info, void* ucontext) {
    sigusr1s_no++;
    sigusr1_catcher_no = info->si_int;
    //printf("Caught SIGUSR1 signal in sender with index: %d.\n", sigusr1_catcher_no);
}

void handle_sigusr2(int sig_no, siginfo_t* info, void* ucontext) {
    //printf("Caught SIGUSR2 signal in sender.\n");
    sigusr2_received = true;
    sigusr1s_received_by_catcher = info->si_int;
}


int main(int argc, char** argv) {

    int sig_no1 = SIGUSR1;
    int sig_no2 = SIGUSR2;

    int catcher_pid = atoi(argv[1]);
    int sigs_no = atoi(argv[2]);
    char* mode = argv[3];

    if (strcmp(mode, "sigrt")==0) {
        sig_no1 = SIGRTMIN+1;
        sig_no2 = SIGRTMIN+2;
    }

    register_sig_handler(sig_no1, handle_sigusr1);
    register_sig_handler(sig_no2, handle_sigusr2);

    for (int i=0;i<sigs_no;i++) {
        send_sig(mode, sig_no1, catcher_pid, 0);
    }

    send_sig(mode, sig_no2, catcher_pid, 0);

    while (!sigusr2_received) {
        ;
    }

    printf("Sender received %d SIGUSR1 signals out of %d.\n", sigusr1s_no, sigs_no);

    if (strcmp(mode, "sigqueue")==0) {
        printf("Last caught SIGUSR1 had index %d\n", sigusr1_catcher_no);
        printf("Info from SIGUSR2: catcher received %d SIGUSR1 signals out of %d.\n", sigusr1s_received_by_catcher, sigs_no);
    }

    return 0;
}
