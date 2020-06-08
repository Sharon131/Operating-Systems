#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "common.h"

#define UNIX_PATH_MAX           108

int server_socket_fd;

char* socket_path_gl;
char* name;

int server_indx = -1;

char game_sign = '-';

char game_board[3][3];
bool move_correct;
int next_move;
pthread_mutex_t move_mutex;

void check_args(int argc, char** argv);

void get_local_server_fd(char* socket_path, char* my_name);
void get_net_server_fd(char* ip_addr, int port_no);

void connect_to_server(char* my_name);

void start_game(struct msg_t* to_read);

void make_a_move(char board[3][3]);
void* get_next_move(void* args);

bool move_allowed(char board[3][3], int move);

void print_board(char board[3][3]);

void finish_game(struct msg_t* to_read);

void respond_to_ping();

void signal_handler(int sig_no);
void at_exit();


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    check_args(argc, argv);
    atexit(at_exit);

    signal(SIGINT, signal_handler);

    char* my_name = argv[1];
    char* connect_mode = argv[2];
    name = my_name;

    if (strcmp(connect_mode, "net") == 0) {    
        char* server_addr = argv[3];
        int port_no = atoi(argv[4]);
        get_net_server_fd(server_addr, port_no);
    } else {
        char* socket_path = argv[3];
        socket_path_gl = socket_path;
        get_local_server_fd(socket_path, my_name);
    }

    connect_to_server(my_name);

    while(true) {
        struct msg_t to_read;
        read(server_socket_fd, (void*)&to_read, sizeof(struct msg_t));

        if (to_read.msg_type == START_GAME) {
            start_game(&to_read);
        } else if (to_read.msg_type == WAIT) {
            printf("Wait for another player.\n");
        } else if(to_read.msg_type == MOVE) {
            make_a_move(to_read.board);
        } else if (to_read.msg_type == FINISHED) {
            finish_game(&to_read);
        } else if (to_read.msg_type == PING) {
            respond_to_ping();
        } else if (to_read.msg_type == DISCONNECT) {
            printf("Disconnecting from server.\n");
            return 0;
        } else {
            printf("Unknown type message: %d. Exiting.\n", to_read.msg_type);
            exit(1);
        }
        
    }

    printf("Hello.\n");

    return 0;
}


/*--------------------------------------------------------------*/


void check_args(int argc, char** argv) {
    if (argc < 4) {
        printf("Not enough arguments. Exiting.\n");
        exit(1);
    }

    if (strcmp(argv[2], "net") == 0 && argc < 5) {
        printf("Not enough arguments. Exiting.\n");
        exit(1);
    }

    if (strcmp(argv[2], "net")!= 0 && strcmp(argv[2], "local")!=0) {
        printf("Unknown mode of connecting to server. Exiting.\n");
        exit(1);
    }
}


void get_local_server_fd(char* socket_path, char* my_name) {
    server_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (server_socket_fd < 0) {
        printf("Error in obtaining socket descriptor. Exiting.\n");
        exit(1);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, UNIX_PATH_MAX);

    struct sockaddr_un addr2;
    addr2.sun_family = AF_UNIX;
    strncpy(addr2.sun_path, my_name, UNIX_PATH_MAX);

    if (bind(server_socket_fd, (struct sockaddr*)&addr2, sizeof(addr)) < 0) {
        printf("Error in binding with server. Exiting.\n");
        exit(1);
    }

    if (connect(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error in connecting with server. Exiting.\n");
        exit(1);
    }
}


void get_net_server_fd(char* ip_addr, int port_no) {
    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_socket_fd < 0) {
        printf("Error in obtaining socket descriptor. Exiting.\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_no);
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    struct sockaddr_in addr2;
    addr2.sin_family = AF_INET;
    addr2.sin_port = 0;
    addr2.sin_addr.s_addr = inet_addr(ip_addr);

    if (bind(server_socket_fd, (struct sockaddr*)&addr2, sizeof(addr)) < 0) {
        printf("Error in binding with server. Exiting.\n");
        exit(1);
    }

    if (connect(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Error in connecting with server. Exiting.\n");
        exit(1);
    }
}


void connect_to_server(char* my_name) {
    struct msg_t to_send;
    to_send.msg_type = CONNECT;
    strncpy(to_send.client_name, my_name, MAX_NAME_LEN);
    
    write(server_socket_fd, (void*)(&to_send), sizeof(to_send));

    //printf("Waiting for response from server.\n");
    struct msg_t to_read;
    read(server_socket_fd, (void*)(&to_read), sizeof(struct msg_t));
    //printf("Response gotten.\n");

    if (to_read.msg_type == CONNECT_AGAIN) {
        printf("There already exists client with that name. Exiting.\n");
        exit(1);
    } else if(to_read.msg_type == CONNECT) {
        server_indx = to_read.client_indx;
        printf("Connected to server.\n");
    } else {
        printf("Error in connecting to server. Exiting.\n");
        exit(1);
    }
    
}


void start_game(struct msg_t* to_read) {
    
    if (to_read->x_sign)    game_sign = 'x';
    else                    game_sign = 'o';

    printf("Game started.\n");

    if (to_read->first_move) {
        make_a_move(to_read->board);
    } else {
        print_board(to_read->board);
        printf("Wait for another player's move.\n");
    }
}


void* get_next_move(void* args) {

    int move = getchar() - '0';

    while (!move_allowed(game_board, move)) {
        move = getchar() - '0';
    }

    pthread_mutex_lock(&move_mutex);
    move_correct = true;
    next_move = move;
    pthread_mutex_unlock(&move_mutex);

    pthread_exit(0);
}


void make_a_move(char board[3][3]) {
    print_board(board);
    printf("Your move. Where do you want to put your sign?\n");
    
    move_correct = false;

    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            game_board[i][j] = board[i][j];
        }
    }

    pthread_t move_thread;
    pthread_create(&move_thread, NULL, get_next_move, NULL);
    
    pthread_mutex_lock(&move_mutex);
    while (!move_correct) {
        pthread_mutex_unlock(&move_mutex);

        struct msg_t to_read;

        read(server_socket_fd, (void*)&to_read, sizeof(struct msg_t));

        if (to_read.msg_type == PING) {
            respond_to_ping();
        } else if (to_read.msg_type == DISCONNECT) {
            printf("Disconnecting from server.\n");
            exit(0);
        } 

        pthread_mutex_lock(&move_mutex);
    }
    pthread_mutex_unlock(&move_mutex);

    int move = next_move;
    pthread_join(move_thread, NULL);

    int row = move/3;
    int col = move%3;

    struct msg_t to_send;
    to_send.client_indx = server_indx;
    to_send.msg_type = MOVE;

    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            if (i == row && j == col) {
                to_send.board[i][j] = game_sign;
            }
            else {
                to_send.board[i][j] = board[i][j];
            }
        }
    }

    print_board(to_send.board);

    write(server_socket_fd, (void*)&to_send, sizeof(struct msg_t));
}


bool move_allowed(char board[3][3], int move) {
    if (move < 0 || move > 8){
        return false;
    }
    
    int row = move/3;
    int col = move%3;

    return (board[row][col] != 'x' && board[row][col] != 'o');
}


void print_board(char board[3][3]) {

    printf("Game board:\n");
    for(int i=0;i<3;i++) {
        printf("%d", i);
        for (int j=0;j<3;j++) {
            printf("\t%c", board[i][j]);
        }
        printf("\n");
    }
}


void finish_game(struct msg_t* to_read) {
    
    if (to_read->winner_sign == game_sign) {
        printf("Congratulations! You won!\n");
    } else {
        print_board(to_read->board);
        printf("Sadly, you lost.\n");
    }

    exit(0);
}


void respond_to_ping() {
    struct msg_t to_send;
    to_send.client_indx = server_indx;
    to_send.msg_type = PING;
    write(server_socket_fd, (void*)&to_send, sizeof(struct msg_t));
}


void signal_handler(int sig_no) {
    exit(0);
}


void at_exit() {
    printf("Ending connection with server.\n");

    struct msg_t to_send;
    to_send.client_indx = server_indx;
    to_send.msg_type = DISCONNECT;

    write(server_socket_fd, (void*)&to_send, sizeof(struct msg_t));

    if (socket_path_gl != NULL) {
        unlink(socket_path_gl);
    }

    unlink(name);
    close(server_socket_fd);
}