#include "common.h"

#define MAX_CLIENTS_NO  100

static short sigint_received = false;

void register_sigint_handler ();
void handle_sigint(int sig_num);


void handle_sigint(int sig_num) {
    sigint_received = true;
}

void register_sigint_handler () {
    struct sigaction act;
    act.sa_handler = handle_sigint;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}


void init_client() {
    ;
}

void list_clients() {
    ;
}

void connect_clients(int first_id, int second_id) {
    ;
}

void diconnect_clients(int first_id, int second_id) {

}

void stop_client(int client_id) {
    ;
}

/*-------------------------------------------------------------*/

int main(int argc, char** argv) {

    // int??
    int clients[MAX_CLIENTS_NO];
    
    //zarejestrowanie obłsgi sygnału sigint

    //tworzenie kolejki dla serwera

    //identyfikator kolejki. Do ustalenia na potem, po utworzeniu
    int queue_id = 10;
    struct msgbuff message;


    while (!sigint_received) {
        msgrcv(queue_id,&message, sizeof(message.mtext), 0 , 0);

        //zmiana formy -> najpierw wyciągamy STOP, potem DISCONECT, potem LIST i resztę
        if ((message.mtype & INIT) == INIT) {
            ;
        }
        if ((message.mtype & LIST) == LIST) {
            ;
        }
        if ((message.mtype & CONNECT) == CONNECT) {
            ;
        }
        if ((message.mtype & DISCONNECT) == DISCONNECT) {
            ;
        }
        if ((message.mtype & STOP) == STOP) {
            ;
        } 
        if (message.mtype > INIT + LIST + CONNECT + DISCONNECT + STOP){
            printf("Message type: %ld not known.\n", message.mtype);
        }
    }

    //procedura zamknięcia serwera

    return 0;
}

/*-------------------------------------------------------------*/
