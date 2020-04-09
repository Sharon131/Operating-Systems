#ifndef _COMMON_
#define _COMMON_

#include <signal.h>

void register_sig_handler(int sig_no, void (*func)(int, siginfo_t *, void *));


void send_sig(char* mode, int sig_no, int catcher_pid, int sigqueue_val);



#endif