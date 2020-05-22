#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

struct queue {
    int* vals;
    pthread_cond_t* cond_vars;
    int size;
    int vals_no;
    int head;
};

int chairs_no;
int clients_no;

bool is_barber_sleeping = false;

struct queue* waiting_clients; 

pthread_cond_t waiting_clients_changed;
pthread_mutex_t mutex;


void check_args(int argc, char** argv);

void init_mutex ();
void init_cond_var();

void create_clients_queue();

void* barber_thread_func(void* args);
void barber_shave_next_client();
void barber_sleep();

void* client_thread_func(void* args);
void client_take_a_sit(int id);

void at_exit();


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    check_args(argc, argv);

    srand(time(NULL));

    chairs_no = atoi(argv[1]);
    clients_no = atoi(argv[2]);

    init_mutex();
    init_cond_var();

    pthread_t barber_thread;
    pthread_create(&barber_thread, NULL, barber_thread_func, NULL);
    
    create_clients_queue();

    int client_delay = rand()%4+2;
    sleep(client_delay);

    pthread_t* clients_threads = calloc(clients_no, sizeof(pthread_t));
    int* clients_args = calloc(clients_no, sizeof(int));

    for (int i=0;i<clients_no;i++) {
        client_delay = rand()%3+1;
        sleep(client_delay);
        clients_args[i] = i;
        pthread_create(&clients_threads[i], NULL, client_thread_func, (void*)&clients_args[i]);
    }

    // wait for clients to be shaved
    for (int i=0;i<clients_no;i++) {
        pthread_join(clients_threads[i], NULL);
    }

    pthread_join(barber_thread, NULL);

    at_exit();

    return 0;
}


/*--------------------------------------------------------------*/


void check_args(int argc, char** argv) {
    if (argc < 3) {
        printf("Not enough anrguments. Exiting.\n");
        exit(1);
    }

    if (atoi(argv[1]) <= 0) {
        printf("Number of chairs in waiting room should be greater than 0. Exiting.\n");
        exit(1);
    }

    if (atoi(argv[2]) <= 0) {
        printf("Number of clients should be greater than 0. Exiting.\n");
        exit(1);
    }
 
 }


void init_mutex () {

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Error in creating mutex for threads. Exiting.\n");
        exit(1);
    }
}


void init_cond_var() {

    if (pthread_cond_init(&waiting_clients_changed, NULL) != 0) {
        printf("Error in creating condition variable. Exiting.\n");
        exit(1);
    }
}


void create_clients_queue() {

    waiting_clients = calloc(1, sizeof(struct queue));
    waiting_clients->head = 0;
    waiting_clients->size = chairs_no;
    waiting_clients->vals_no = 0;
    waiting_clients->vals = calloc(chairs_no, sizeof(int));
    waiting_clients->cond_vars = calloc(chairs_no, sizeof(pthread_cond_t));

    for (int i=0;i<chairs_no;i++) {
        if (pthread_cond_init(&(waiting_clients->cond_vars[i]), NULL) != 0) {
            printf("Error in creating condition variables for clients. Exiting.\n");
            exit(1);
        }    
    }
    
}


void barber_shave_next_client() {
    
    int waiting_no = waiting_clients->vals_no-1;
    int client_id = waiting_clients->vals[waiting_clients->head];
    pthread_cond_t* cond_var = &waiting_clients->cond_vars[waiting_clients->head];

    waiting_clients->head = (waiting_clients->head + 1)%chairs_no;
    waiting_clients->vals_no--;

    pthread_mutex_unlock(&mutex);

    printf("Barber: %d clients are still waiting. Shaving client with id: %d\n", waiting_no, client_id);
    int time_needed = rand()%3+2;
    sleep(time_needed);

    pthread_cond_broadcast(cond_var);
}


void barber_sleep() {
    printf("Barber: going to sleep.\n");
    pthread_cond_wait(&waiting_clients_changed, &mutex);  
    pthread_mutex_unlock(&mutex);      
}


void* barber_thread_func(void* args) {

    int indx = 0;
    while (indx < clients_no) {
        if (pthread_mutex_trylock(&mutex)==0) {
            if (waiting_clients->vals_no > 0) {
                barber_shave_next_client();
                indx++;
            } else {
                is_barber_sleeping = true;
                barber_sleep();
                is_barber_sleeping = false;
            }

        }
    }

    return 0;
}


void client_take_a_sit(int id) {
    int tail = (waiting_clients->head+waiting_clients->vals_no)%chairs_no;
    waiting_clients->vals[tail] = id;
    waiting_clients->vals_no++;
}


void* client_thread_func(void* args) {

    int id = ((int*)args)[0];

    bool is_shaved = false;

    while (!is_shaved) {
        if (pthread_mutex_trylock(&mutex)==0) {

            int waiting_no = waiting_clients->vals_no;
            int tail = (waiting_clients->head+waiting_no)%chairs_no;
            pthread_cond_t* wait_till_shaved = &(waiting_clients->cond_vars[tail]);

            if (waiting_no < chairs_no) {
                
                client_take_a_sit(id);
                
                if (waiting_no == 0 && is_barber_sleeping) {
                    printf("Client: Waking up barber. ID: %d\n", id);
                    pthread_cond_broadcast(&waiting_clients_changed);
                } else {
                    printf("Client: Waiting room, free chairs: %d, ID: %d\n", chairs_no-waiting_no-1, id);
                }

                pthread_cond_wait(wait_till_shaved, &mutex);
                is_shaved = true;
                
            } else {
                printf("Client: No free chairs in waiting room. ID: %d\n", id);
                
            }

            pthread_mutex_unlock(&mutex);

            if (waiting_no >= chairs_no) {
                int wait = rand()%10+4;
                sleep(wait);
            }
        }
    }

    return 0;
}


void at_exit() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&waiting_clients_changed);

    free(waiting_clients->vals);
    free(waiting_clients);
}

