#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>

void register_handler(int sig_no);


void signal_handler(int sig_no, siginfo_t* info, void* ucontext) {
    printf("Errno val: %d\n", info->si_errno);
    printf("Signal code val: %d\n", info->si_code);
    printf("Real UID of sending process: %d.\n", info->si_uid);
    printf("User time taken: %ld\n", info->si_utime);
    printf("System time taken: %ld\n", info->si_stime);

    printf("File descriptor: %d.\n", info->si_fd);
}



int main(int argc, char** argv) {

    int sig_no = SIGINT;
    register_handler(sig_no);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig_no);
    sigprocmask(SIG_BLOCK, &set, NULL);

    int fp = open("text.txt", O_RDWR);

    raise(sig_no);

    sleep(3);

    sigset_t set2;
    sigemptyset(&set2);
    //sigaddset(&set2, sig_no);
    sigsuspend(&set2);

    close(fp);
    
    return 0;
}

void register_handler(int sig_no) {
    struct sigaction act;
    act.sa_sigaction = signal_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(sig_no, &act, NULL);

}
