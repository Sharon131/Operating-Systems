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

#define MAX_MSG_TYPE    64
#define INIT            32
#define LIST            16
#define MESSAGE         8
#define CONNECT         4
#define DISCONNECT      2
#define STOP            1

#define SERVER_KEY_ID   13

#define MAX_MSG_LEN     501
#define SIZEOF_BUFF     (sizeof(struct msgbuff)-sizeof(long))

#define IPC_FLAGS        S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH


struct client_info {
    int queue_id;
    bool is_chatting;
    int client_id;
    //struct client_info* next;
};

struct msgbuff {
    long mtype;
    key_t sender_key;
    int client_id;
    int connect_to;
    int queue_id_to_connect;
    struct client_info clients;
    char message[MAX_MSG_LEN];
};

#endif