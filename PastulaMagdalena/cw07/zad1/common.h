#ifndef COMMON
#define COMMON

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_ORDERS_NO   10
#define SEMAPHORES_NO   3

#define ORDERS_SEM      0
#define PACKER_SEM      1
#define SENDER_SEM      2

#define SEM_KEY_ID      21
#define MEM_KEY_ID      22

union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

struct orders {
    int orders[MAX_ORDERS_NO];
    int receiver_index;
    int packer_index;
    int sender_index;
};




#endif