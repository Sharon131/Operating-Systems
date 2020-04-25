#include "common.h"


static short sigint_received = false;


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

void chat_mode() {
    ;
}

int main(int argc, char** argv) {

    int server_queue_id = atoi(argv[1]);

    //zarejestrowanie obsługi sygnału sigint

    // tworzenie kolejki dla klienta

    // wysłanie zlecenia init do serwera
    
    //losowanie dalszych akcji??
    while(!sigint_received) {
        ; //??
    }

    return 0;
}