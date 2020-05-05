#ifndef COMMON
#define COMMON

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_ORDERS_NO   100

#define SEM_KEY_ID      13
#define MEM_KEY_ID      10

#endif