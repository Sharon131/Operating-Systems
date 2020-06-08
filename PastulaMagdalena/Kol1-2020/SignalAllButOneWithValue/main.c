#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <string.h>

int sig_handled = 0;

void sighandler(int sig_no, siginfo_t* sig_info, void* ucontext) {
    sig_handled = 1;
    printf("Otrzymana wartość: %d\n",  sig_info->si_value);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;
    sigset_t set;
    sigemptyset(&set);
    action.sa_mask = set;

    sigaction(SIGUSR1, &action, NULL);

    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
        sigset_t child_set;
        sigfillset(&child_set);
        sigdelset(&child_set, SIGUSR1);

        sigprocmask(SIG_BLOCK, &child_set, NULL);

    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        union sigval val;
        val.sival_int = atoi(argv[1]);
        sigqueue(child, atoi(argv[2]), val);
    }

    return 0;
}
