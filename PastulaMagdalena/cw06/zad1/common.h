#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>

#define MAX_MSG_NUM     32
#define INIT            16
#define LIST            8
#define CONNECT         4
#define DISCONNECT      2
#define STOP            1

#define SERVER_KEY_ID   13

#define MAX_MSG_LEN     4096
#define SIZEOF_BUFF     (sizeof(struct msgbuff)-sizeof(long))

#define IPC_FLAGS        S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH


struct client_info {
    int queue_id;
    bool is_chatting;
    int client_id;
    struct client_info* next;
};

struct msgbuff {
    long mtype;
    key_t sender_key;
    int client_id;
    int connect_to;
    key_t key_to_connect;
    struct client_info clients;
};

#endif