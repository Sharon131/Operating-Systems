#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>

#define INIT        1
#define LIST        2
#define CONNECT     4
#define DISCONNECT  8
#define STOP        16

struct msgbuff {
    long mtype;
    void* mtext;
};

#endif