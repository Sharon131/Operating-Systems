#include <time.h>

#include "common.h"

static bool sigint_received = false;

key_t this_key = 0;
static int this_queue_id = 0;
static int this_id = 0;
static int server_queue_id;

static int other_queue_id = 0;
static int other_client_id = 0;
static bool is_chatting = false;

static struct msgbuff* response;
static struct msgbuff to_send;

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

key_t create_IPCqueue_and_assert();
int get_server_queue_id ();

void init_connection_with_server();
void get_list_of_clients();
void start_chat_with(int other_client_id);

void handle_received_message(struct msgbuff* msg);
void handle_user_command(char* buffer, int len);


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {
    
    srand(time(NULL));
    response = calloc(1, sizeof(struct msgbuff));

    atexit(at_exit);
    register_sigint_handler();

    this_key = create_IPCqueue_and_assert();

    server_queue_id = get_server_queue_id();
    
    printf("Client key: %d\n", this_key);
    printf("Client queue id: %d\n", this_queue_id);

    init_connection_with_server();

    //po połączeniu z innym klientem wysyłąnie wiadomości
    //odczytanych w terminalu
    char* buffer = NULL;
    size_t len = 0;    

    printf("Ready to work.\n");
    printf("Waiting for your commands.\n");
    printf("Available commands:\n");
    printf("- LIST -> listing current chat users.\n");
    printf("- CONNECT -> connect with user whose id you type after that command.\n");
    printf("- DISCONNECT -> disconnect from current user char.\n");
    printf("- STOP -> exit chat.\n\n");

    while(!sigint_received) {

        if (msgrcv(this_queue_id, response, SIZEOF_BUFF, -1*MAX_MSG_TYPE, IPC_NOWAIT | IPC_FLAGS) != -1 ) {
            handle_received_message(response);    
        }        

        int char_no = getline(&buffer, &len, stdin);
        
        if (char_no > 0 ) {
            buffer[char_no-1] = 0;
            handle_user_command(buffer, char_no);
        }

    }

    
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
    printf("\nShutting down the client.\n");

    to_send.mtype = STOP;
    to_send.client_id = this_id;
    to_send.sender_key = this_key;
    msgsnd(server_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
    
    msgctl(this_queue_id, IPC_RMID, NULL);
    free(response);
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

    printf("Other chat users:\n");
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

void start_chat_with(int other_client_id) {
    
    to_send.mtype = CONNECT;
    to_send.sender_key = this_key;
    to_send.client_id = this_id;
    to_send.connect_to = other_client_id;

    msgsnd(server_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);

    msgrcv(this_queue_id, response, SIZEOF_BUFF, CONNECT, IPC_FLAGS);

    is_chatting = true;

    other_queue_id = response->queue_id_to_connect;
}


void handle_received_message(struct msgbuff* msg) {
    
    if (msg->mtype == STOP) {
        exit(0);
    } else if (msg->mtype == DISCONNECT) {
        is_chatting = false;
        printf("The other user has left the chat.\n");
    } else if (msg->mtype == MESSAGE) {
        printf("User %d: %s \n", other_client_id, msg->message);
    } else if (msg->mtype == CONNECT) {
        other_queue_id = msg->queue_id_to_connect;
        other_client_id = msg->connect_to;
        is_chatting = true;
        printf("User with id %d connected with you.\n", msg->connect_to);
        printf("Remember, sometimes you'll need to pres enter a few times, so that anwear would pop up.\n");
        printf("Also, all commends from main menu still work in here.\n");
    } else {
        printf("Command type: %ld not known.\n", msg->mtype);
    }
}


void handle_user_command(char* buffer, int len) {
    if (strcmp(buffer, "LIST") == 0) {
        get_list_of_clients();
    } else if(strcmp(buffer, "CONNECT") == 0) {
        printf("Please, enter id of user you want to connect to:\n");
        size_t size;
        int char_no = getline(&buffer, &size, stdin);
        buffer[char_no-1] = 0;
        int connect_to = atoi(buffer);

        if (connect_to >= 0) {
            start_chat_with(connect_to);
            printf("Chat with user %d began.\n", connect_to);
            printf("Remember, sometimes you'll need to pres enter a few times, so that anwear would pop up.\n");
            printf("Also, all commends from main menu still work in here.\n");
        } else {
            printf("User id cannot be less than 0. Cannot connect.\n");
        }
    } else if(strcmp(buffer, "DISCONNECT") == 0) {
        is_chatting = false;
        to_send.mtype = DISCONNECT;
        to_send.connect_to = other_client_id;
        msgsnd(server_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
    } else if(strcmp(buffer, "STOP") == 0) {
        if (is_chatting) {
            to_send.mtype = DISCONNECT;
            msgsnd(server_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
        }
        exit(0);
    } else if (is_chatting && len > 2) {
        to_send.mtype = MESSAGE;
        to_send.client_id = this_id;
        to_send.sender_key = this_key;
        strncpy(to_send.message, buffer, MAX_MSG_LEN-1);
        msgsnd(other_queue_id, &to_send, SIZEOF_BUFF, IPC_FLAGS);
    } else {
        if (len > 1){
            printf("Command: %s is not know. Try again and do not write any additional white marks.\n", buffer);
        }
    }
}