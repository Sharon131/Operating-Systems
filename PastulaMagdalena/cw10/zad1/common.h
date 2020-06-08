#ifndef _COMMON_
#define _COMMON_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>

#define MAX_NAME_LEN        10

enum msg_type {
    CONNECT,
    CONNECT_AGAIN,
    WAIT,
    START_GAME,
    MOVE,
    FINISHED,
    PING,
    DISCONNECT
};


struct msg_t {
    enum msg_type msg_type;
    char client_name[MAX_NAME_LEN+1];
    //int clients_move;
    //int move;           //?
    bool first_move;
    bool x_sign;
    char board[3][3];
    char winner_sign;
};

#endif