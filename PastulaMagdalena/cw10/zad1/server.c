#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "common.h"

#define UNIX_PATH_MAX           108

#define MAX_WAITING_CLIENTS     15
#define MAX_CLIENTS_NO          15

struct room_t {
    int player1_indx;
    int player2_indx;
    char board[3][3];
};

struct client_t {
    bool exists;
    bool in_game;
    bool ping_rcvd;
    int socket_fd;
    char* client_name;
    struct room_t* game_room;
};

struct client_t* clients[MAX_CLIENTS_NO];
pthread_mutex_t clients_mutex;

pthread_t listen_thread;
pthread_t ping_thread;

char* socket_path;
int local_socket_fd;
int global_socket_fd;

void check_args(int argc, char** argv);
void prepare_clients_list();


void create_local_socket(char* socket_path);
void create_global_socket(int port_no);

void* ping_thread_func(void* args);
void* listen_thread_func(void* args);

void reset_polls(struct pollfd* polls);

void handle_received_msg(int fd, int client_indx);

void next_move(struct msg_t* to_read, int client_indx);
char is_game_finished(char board[3][3]);


void accept_any_new_clients(struct pollfd* polls);
void add_new_client(int fd);

bool exists_with_name(char* name);

void try_to_match_with(int indx1);
void init_game(int indx1, int indx2);

void disconnect_client(int client_index, bool send_msg);

void signal_handler(int sig_no);
void at_exit();



/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    check_args(argc, argv);
    atexit(at_exit);
    signal(SIGINT, signal_handler);

    prepare_clients_list();

    int port_no = atoi(argv[1]);
    socket_path = argv[2];

    create_local_socket(socket_path);
    create_global_socket(port_no);

    pthread_create(&listen_thread, NULL, listen_thread_func, NULL);
    pthread_create(&ping_thread, NULL, ping_thread_func, NULL);


    pthread_join(listen_thread, NULL);
    pthread_join(ping_thread, NULL);
    
    return 0;
}


/*--------------------------------------------------------------*/


void check_args(int argc, char** argv) {
    if (argc < 3) {
        printf("Not enough arguments. Exiting.\n");
        exit(-1);
    }

}


void prepare_clients_list() {

    for(int i=0;i<MAX_CLIENTS_NO;i++) {
        clients[i] = calloc(1, sizeof(struct client_t));
    }

    if (pthread_mutex_init(&clients_mutex, NULL) < 0) {
        printf("Error in creating clients mutex. Exiting.\n");
        exit(1);
    }
}


void create_local_socket(char* socket_path) {
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, UNIX_PATH_MAX-1);
    
    local_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (local_socket_fd < 0) {
        printf("Error in creating local socket. Exiting.\n");
        exit(1);
    }

    if (bind(local_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error in binding local socket. Exiting.\n");
        exit(1);
    }

    if (listen(local_socket_fd, MAX_WAITING_CLIENTS) < 0) {
        printf("Error in listening local socket in server. Exiting.");
        exit(1);
    }
}


void create_global_socket(int port_no) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_no);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    global_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (global_socket_fd < 0) {
        printf("Error in creating global socket. Exiting.\n");
        exit(1);
    }

    if (bind(global_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error in binding global socket. Exiting.\n");
        exit(1);
    }

    if (listen(global_socket_fd, MAX_WAITING_CLIENTS) < 0) {
        printf("Error in listening global socket in server. Exiting.");
        exit(1);
    }

}


void* ping_thread_func(void* args) {
    
    struct msg_t to_send;
    to_send.msg_type = PING;

    while(true) {
        sleep(1);

        for (int i=0;i<MAX_CLIENTS_NO; i++) {
            
            pthread_mutex_lock(&clients_mutex);
            
            if (clients[i]->exists) {
                clients[i]->ping_rcvd = false;

                send(clients[i]->socket_fd, (void*)&to_send, sizeof(to_send), MSG_DONTWAIT);

                pthread_mutex_unlock(&clients_mutex);

                sleep(1);

                pthread_mutex_lock(&clients_mutex);

                if (!clients[i]->ping_rcvd) {
                    disconnect_client(i, false);
                }

            }
            
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    return 0;
}


void* listen_thread_func(void* args) {
    
    struct pollfd polls[MAX_CLIENTS_NO];
    polls[MAX_CLIENTS_NO].fd = local_socket_fd;
    polls[MAX_CLIENTS_NO+1].fd = global_socket_fd;

    while (true) {

        reset_polls(polls);

        if (poll(polls, MAX_CLIENTS_NO+2, -1) < 0) {
            printf("Error in polling. Exiting.\n");
            exit(1);
        }

        pthread_mutex_lock(&clients_mutex);

        for (int i=0;i<MAX_CLIENTS_NO;i++) {
            if(polls[i].revents & POLLIN){
                
                handle_received_msg(polls[i].fd, i);
                
            } else if (polls[i].revents & POLLHUP){
                disconnect_client(i, false);
            }
        }
        
        accept_any_new_clients(polls);

        pthread_mutex_unlock(&clients_mutex);
    }


    return 0;
}


void reset_polls(struct pollfd* polls) {

    pthread_mutex_lock(&clients_mutex);
    
    for(int i=0;i<MAX_CLIENTS_NO;i++) {
        if (clients[i]->exists) {
            polls[i].fd = clients[i]->socket_fd;    
        } else {
            polls[i].fd = -1;
        }
        polls[i].events = POLLIN;
        polls[i].revents = 0;
    }

    polls[MAX_CLIENTS_NO].events = POLLIN;
    polls[MAX_CLIENTS_NO].revents = 0;
    polls[MAX_CLIENTS_NO+1].events = POLLIN;
    polls[MAX_CLIENTS_NO+1].revents = 0;

    pthread_mutex_unlock(&clients_mutex);
}


void handle_received_msg(int fd, int client_indx) {
    struct msg_t to_read;
    read(fd, (void*)&to_read, sizeof(struct msg_t));

    if (to_read.msg_type == MOVE) {
        next_move(&to_read, client_indx);
    } else if (to_read.msg_type == PING) {
        clients[client_indx]->ping_rcvd = true;
    } else if(to_read.msg_type == DISCONNECT) {
        disconnect_client(client_indx, false);
    }
}


void next_move(struct msg_t* to_read, int client_indx) {
    int other_player;

    if (clients[client_indx]->game_room->player1_indx == client_indx) {
        other_player = clients[client_indx]->game_room->player2_indx;
    } else {
        other_player = clients[client_indx]->game_room->player1_indx;
    }

    struct msg_t to_send;
    struct msg_t to_send2;

    for (int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            to_send.board[i][j] = to_read->board[i][j];
        }
    }

    char sign;
    if ((sign = is_game_finished(to_read->board)) != '-') {
        
        to_send.msg_type = FINISHED;
        to_send2.msg_type = FINISHED;

        to_send.winner_sign = sign;
        to_send2.winner_sign = sign;

        write(clients[other_player]->socket_fd, (void*)&to_send, sizeof(struct msg_t));
        write(clients[client_indx]->socket_fd, (void*)&to_send2, sizeof(struct msg_t));

        free(clients[client_indx]->game_room);
        clients[client_indx]->game_room = NULL;
        clients[other_player]->game_room = NULL;

        clients[client_indx]->in_game = false;
        clients[other_player]->in_game = false;

        printf("Client %d and %d finished game.\n", client_indx, other_player);

    } else {

        to_send.msg_type = MOVE;
        to_send2.msg_type = WAIT;

        write(clients[other_player]->socket_fd, (void*)&to_send, sizeof(struct msg_t));
        write(clients[client_indx]->socket_fd, (void*)&to_send2, sizeof(struct msg_t));
    }

}


char is_game_finished(char board[3][3]) {

    for (int i=0;i<3;i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2]){
            if (board[i][0] != '-'){
                return board[i][0];
            }
        }
    }

    for (int j=0;j<3;j++) {
        if (board[0][j] == board[1][j] && board[0][j] == board[2][j]) {
            if (board[0][j] != '-'){
                return board[0][j];
            }
        }
    }

    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        if (board[0][0] != '-') {
            return board[0][0];
        }
    }

    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        if (board[0][2] != '-') {
            return board[0][2];   
        }
    }

    return '-';
}


void accept_any_new_clients(struct pollfd* polls) {

    if (polls[MAX_CLIENTS_NO].revents == POLLIN) {
        add_new_client(polls[MAX_CLIENTS_NO].fd);
    }
        
    if (polls[MAX_CLIENTS_NO+1].revents == POLLIN) {
        add_new_client(polls[MAX_CLIENTS_NO+1].fd);
    }
}


void add_new_client(int fd) {
    int client_fd = accept(fd, NULL, NULL);

    if (client_fd < 0) {
        printf("Error in accepting new client. Exiting.\n");
        exit(1);
    }

    struct msg_t to_read;
    read(client_fd, (void*)(&to_read), sizeof(struct msg_t));

    int indx = 0;
    while (clients[indx]->exists && indx < MAX_CLIENTS_NO) {
        indx++;
    }

    struct msg_t to_send;

    if (!exists_with_name(to_read.client_name)) {
        clients[indx]->socket_fd = client_fd;
        clients[indx]->exists = true;
        clients[indx]->ping_rcvd = false;
        clients[indx]->in_game = false;
        clients[indx]->client_name = calloc(strlen(to_read.client_name)+1, sizeof(char));
        strcpy(clients[indx]->client_name, to_read.client_name);
        
        to_send.msg_type = CONNECT;

        printf("Client %s added.\n", clients[indx]->client_name);
    } else {
        to_send.msg_type = CONNECT_AGAIN;

        printf("Adding client %s failed.\n", to_read.client_name);
    }

    write(client_fd, (void*)(&to_send), sizeof(struct msg_t));

    try_to_match_with(indx);
}


bool exists_with_name(char* name) {

    for (int i=0;i<MAX_CLIENTS_NO;i++) {
        if (clients[i]->exists && strcmp(clients[i]->client_name, name)==0){
            return true;
        }
    }

    return false;
}


void try_to_match_with(int indx1) {

    int indx2 = -1;

    for (int i=0;i<MAX_CLIENTS_NO;i++) {
        if (clients[i]->exists && !clients[i]->in_game) {
            if (indx2 == -1 && i!=indx1){
                indx2 = i;
            }
        }
    }

    if (indx2 >= 0) {
        init_game(indx1, indx2);
    } else {
        struct msg_t to_send;
        to_send.msg_type = WAIT;
        write(clients[indx1]->socket_fd, (void*)&to_send, sizeof(struct msg_t));
        printf("Client %d is waiting for another client.\n", indx1);
    }

}


void init_game(int indx1, int indx2) {
    bool should_start = rand()%2;
    bool x_sign = rand()%2;

    struct msg_t to_send;
    struct msg_t to_send2;

    to_send2.msg_type = START_GAME;
    to_send.msg_type = START_GAME;

    clients[indx2]->game_room = calloc(1, sizeof(struct room_t));
    clients[indx1]->game_room = clients[indx2]->game_room;
    clients[indx2]->game_room->player1_indx = indx1;
    clients[indx2]->game_room->player2_indx = indx2;
    clients[indx2]->in_game = true;
    clients[indx1]->in_game = true;

    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            clients[indx2]->game_room->board[i][j] = '-';
            to_send.board[i][j] = '-';
            to_send2.board[i][j] = '-';
        }
    }

    to_send.first_move = should_start;
    to_send.x_sign = x_sign;

    write(clients[indx2]->socket_fd, (void*)&to_send, sizeof(struct msg_t));    

    
    to_send2.first_move = !should_start;
    to_send2.x_sign = !x_sign;  
    
    write(clients[indx1]->socket_fd, (void*)&to_send2, sizeof(struct msg_t));

    printf("Matched client %d with %d\n", indx1, indx2);  

}


void disconnect_client(int client_index, bool send_msg) {
    if (send_msg){
        struct msg_t to_send;
        to_send.msg_type = DISCONNECT;

        write(clients[client_index]->socket_fd, (void*)&to_send, sizeof(struct msg_t));    
    }
    
    if (clients[client_index]->in_game) {
        int other_player;

        if (clients[client_index]->game_room->player1_indx == client_index) {
            other_player = clients[client_index]->game_room->player2_indx;
        } else {
            other_player = clients[client_index]->game_room->player1_indx;
        }

        struct msg_t to_send;
        to_send.msg_type = DISCONNECT;
        write(clients[other_player]->socket_fd, (void*)&to_send, sizeof(struct msg_t));
    }

    free(clients[client_index]->client_name);

    shutdown(clients[client_index]->socket_fd, SHUT_RDWR);
    close(clients[client_index]->socket_fd);

    clients[client_index]->exists = false;
    printf("Client %d disconnected.\n", client_index);
}


void signal_handler(int sig_no) {
    exit(0);
}


void at_exit() {

    printf("Closing server.\n");

    pthread_cancel(ping_thread);
    pthread_cancel(listen_thread);

    for(int i=0;i<MAX_CLIENTS_NO;i++) {
        if (clients[i]->exists) {
            disconnect_client(i, true);
        }

        free(clients[i]);
    }

    shutdown(local_socket_fd, SHUT_RDWR);
    shutdown(global_socket_fd, SHUT_RDWR);

    close(local_socket_fd);
    close(global_socket_fd);

    unlink(socket_path);
}
