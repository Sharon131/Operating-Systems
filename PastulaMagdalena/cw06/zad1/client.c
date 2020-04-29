#include <time.h>

#include "common.h"


static short sigint_received = false;
key_t this_key = 0;
static int this_queue_id = 0;
static int this_id = 0;
static int server_queue_id;

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

key_t create_IPCqueue_and_assert();
int get_server_queue_id ();

void init_connection_with_server();
void get_list_of_clients();

key_t start_chat_with(int other_client_id) {
    
    return 0;
}

void chat_mode() {
    ;
}


void handle_message(struct msgbuff* msg) {
    if (msg->mtype == STOP) {
        sigint_received = true;
    } /*else if (msg->mtype == DISCONNECT) {
        ;
    } */else {
        printf("Command type: %ld= S not known.\n", msg->mtype);
    }
}


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {
    
    srand(time(NULL));
    
    atexit(at_exit);
    register_sigint_handler();

    this_key = create_IPCqueue_and_assert();

    server_queue_id = get_server_queue_id();
    
    printf("Client key: %d\n", this_key);
    printf("Client queue id: %d\n", this_queue_id);

    init_connection_with_server();

    get_list_of_clients();

    //po połączeniu z innym klientem wysyłąnie wiadomości
    //odczytanych w terminalu
    while(!sigint_received) {
        struct msgbuff* msg = calloc(1, sizeof(struct msgbuff));
        if (msgrcv(this_queue_id, msg, 0, -1*MAX_MSG_NUM, IPC_NOWAIT | IPC_FLAGS) != -1) {
            handle_message(msg);    
        }
        free(msg);
    }

    printf("\nShutting down the client.\n");

    return 0;
}


/*--------------------------------------------------------------*/


void register_sigint_handler () {
    struct sigaction act;
    act.sa_handler = handle_sigint;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

void handle_sigint(int sig_num) {
    if (!sigint_received) {
        sigint_received = true;
    } else {
        exit(0);
    }
}

void at_exit(void) {
    struct msgbuff to_send;
    to_send.mtype = STOP;
    to_send.client_id = this_id;
    to_send.sender_key = this_key;
    msgsnd(server_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
    
    msgctl(this_queue_id, IPC_RMID, NULL);
}

key_t create_IPCqueue_and_assert() {
    
    key_t key = ftok(getenv("HOME"), rand()%256);
    this_queue_id = msgget(key, IPC_CREAT | IPC_EXCL | IPC_FLAGS);
    
    if (this_queue_id < 0) {
        printf("Error in creating queue for client. Exiting.\n");
        exit(-1);
    }

    return key;
}

int get_server_queue_id () {
    int server_queue_id = msgget(ftok(getenv("HOME"), SERVER_KEY_ID), 0);

    if (server_queue_id == -1) {
        printf("Error in obtaining server queue id.\n");
    }

    return server_queue_id;
}


void init_connection_with_server() {
    
    struct msgbuff message;
    
    message.mtype = INIT;
    message.sender_key = this_key;
    
    if (msgsnd(server_queue_id, &message, SIZEOF_BUFF, IPC_FLAGS) < 0 ) {
        printf("Error in sending command INIT to server. Exiting.\n");
        exit(-1);
    }
    
    struct msgbuff* msg = calloc(1, sizeof(struct msgbuff));
    
    if (msgrcv(this_queue_id, msg, SIZEOF_BUFF, INIT, IPC_FLAGS) < 0) {
        printf("Error in obtaining response for INIT command. Exiting.\n");
        exit(-1);
    }
    
    this_id = msg->client_id;

    free(msg);
}


void get_list_of_clients() {
    struct msgbuff msg;
    msg.mtype = LIST;
    msg.sender_key = this_key;
    msg.client_id = this_id;

    if (msgsnd(server_queue_id, &msg, SIZEOF_BUFF, IPC_FLAGS) < 0) {
        printf("Error in sending command LIST to server. Exiting.\n");
        exit(-1);
    }

    struct msgbuff* msg_reply = calloc(1, sizeof(struct msgbuff));

    int code = msgrcv(this_queue_id, msg_reply, SIZEOF_BUFF, LIST, IPC_FLAGS);

    struct client_info client = msg_reply->clients;

    printf("Other char users:\n");
    while (client.client_id >= 0 && code >= 0) {
        if (client.client_id != this_id) {
            printf("Id: %d, busy: %d\n", client.client_id, client.is_chatting);
        } else {
            printf("Id: %d -> that's you.\n", client.client_id);
        }

        code = msgrcv(this_queue_id, msg_reply, SIZEOF_BUFF, LIST, IPC_FLAGS);
        client = msg_reply->clients;
    }

    printf("No other chat users or there's been an error in receiving users list.\n");
    free(msg_reply);
}