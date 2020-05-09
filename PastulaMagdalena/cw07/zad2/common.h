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
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_ORDERS_NO   10
#define SEMAPHORES_NO   3

#define ORDERS_SEM      0
#define PACKER_SEM      1
#define SENDER_SEM      2

#define SEM_NAME        "/mem_semaphore"
#define MEM_NAME        "/orders_mem"

struct orders {
    int orders[MAX_ORDERS_NO];
    int receiver_index;
    int packer_index;
    int sender_index;
    int to_pack_no;
    int to_send_no;
};




#endif