#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>


void handle_SIGTSTP(int sig_num) {
    static int indx = 0;
    printf("\nReceived signal SIGTSTP.\n");

    indx = indx ^ 1;

    if (indx == 1) {
        printf("\nWaiting for ctr+z to continue or ctr+c to end.\n");
        
        sigset_t sigs;
        sigfillset(&sigs);
        sigdelset(&sigs, SIGTSTP);
        sigdelset(&sigs, SIGINT);
        sigsuspend(&sigs);
    } 
}

void handle_SIGINT(int sig_num) {
    printf("\nReceived signal SIGINT. Killing.\n");
    exit(0);
}

void register_tstp_handler() {
    signal(SIGTSTP, handle_SIGTSTP);
}

void register_int_handler() {
    struct sigaction act;
    act.sa_handler = handle_SIGINT;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

int main(int argc, char** argv) {
    
    register_tstp_handler();
    register_int_handler();

    while (true) {
        system("ls -l");
        sleep(1);
    }    

    return 0;
}