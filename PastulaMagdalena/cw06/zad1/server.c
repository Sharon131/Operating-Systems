#include "common.h"
#include <errno.h>

#define MAX_CLIENTS_NO  100

static short sigint_received = false;

static key_t this_key = 0;
static int queue_id = 0;

static int clients_no = 0;
//static struct client_info* info = NULL; //correct to static array? and check

static struct client_info* clients_info[MAX_CLIENTS_NO];

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

void create_IPCqueue();


void init_client(struct msgbuff* msg);
void list_clients(struct msgbuff* msg);

void stop_client(struct msgbuff* msg);


void connect_clients(struct msgbuff* msg) {
    ;
}

void diconnect_client(struct msgbuff* msg) {
    clients_info[msg->client_id]->is_chatting = false;
}


void handle_message(struct msgbuff* message) {
    if (message->mtype == INIT) {
        init_client(message);
    } else if(message->mtype == LIST) {
        list_clients(message);
    } else if (message->mtype == CONNECT) {
        ;
    } else if (message->mtype == DISCONNECT) {
        ;
    } else if(message->mtype == STOP) {
        stop_client(message);
    } else {
        printf("Command type %ld not known.\n", message->mtype);
    }
}


/*-------------------------------------------------------------*/


int main(int argc, char** argv) {
    
    atexit(at_exit);
    register_sigint_handler();

    create_IPCqueue();

    printf("Key: %d\n", this_key);
    printf("Queue id: %d\n", queue_id);


    
    while (!sigint_received) {
        struct msgbuff* message = calloc(1, sizeof(struct msgbuff));
        if (msgrcv(queue_id, message, SIZEOF_BUFF, -1*MAX_MSG_NUM, IPC_NOWAIT | IPC_FLAGS) != -1) {
            handle_message(message);    
        }
        free(message);
    }

    printf("\nShutting down the server.\n");

    return 0;
}


/*-------------------------------------------------------------*/


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
    struct msgbuff msg;
    msg.mtype = STOP;
        
    for (int i=0; i<clients_no;i++) {
        if (clients_info[i] != NULL) {
            msgsnd(clients_info[i]->queue_id, &msg, 0, IPC_FLAGS);
            free(clients_info[i]);
        }
    }

    msgctl(queue_id, IPC_RMID, NULL);
}

void create_IPCqueue() {
    this_key = ftok(getenv("HOME"), SERVER_KEY_ID);
    queue_id = msgget(this_key, IPC_CREAT | IPC_EXCL | IPC_FLAGS);

    if (queue_id == -1) {
        printf("Error in creating queue for server. Exiting.\n");
        exit(-1);
    }
}


void init_client(struct msgbuff* msg) {
    
    int client_queue_id = msgget(msg->sender_key, IPC_FLAGS);

    if (client_queue_id == -1) {
        printf("Received client key is not valid.\n");

        if (errno == EACCES) {
            printf("Cannot access.\n");
        } else if(errno == ENOENT) {
            printf("Does not exists.\n");
        }

        return;
    }

    int i;
    for (i=0;i<MAX_CLIENTS_NO && clients_info[i] != NULL;i++);

    struct client_info* client = calloc(1, sizeof(struct client_info));
    
    client->queue_id = client_queue_id;
    client->is_chatting = false;
    client->queue_id = msgget(msg->sender_key, IPC_FLAGS);
    client->client_id = i;
    
    clients_info[i] = client;

    struct msgbuff message;
    message.mtype = INIT;
    message.sender_key = this_key;
    message.client_id = i;
    
    msgsnd(client_queue_id, &message, SIZEOF_BUFF, IPC_FLAGS);

    clients_no++;
}


void list_clients(struct msgbuff* msg) {
    struct msgbuff to_send;
    to_send.mtype = LIST;
    to_send.sender_key = this_key;
    
    int sender_queue_id = msgget(msg->sender_key, 0);

    if (sender_queue_id < 0) {
        printf("Key of sender is not valid. Cannot send reply.\n");
        return;
    }

    for (int i=0;i<MAX_CLIENTS_NO;i++) {
        if (clients_info[i] != NULL) {
            to_send.clients.client_id = clients_info[i]->client_id;
            to_send.clients.is_chatting = clients_info[i]->is_chatting;
            msgsnd(sender_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
        }
    }

    to_send.clients.client_id = -1;
    msgsnd(sender_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
}

void stop_client(struct msgbuff* msg) {

    free(clients_info[msg->client_id]);
    clients_info[msg->client_id] = NULL;

    printf("Deleted client with id: %d from server.\n", msg->client_id   );
}
