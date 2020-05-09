#include "common.h"

int semaphore_id;
int shared_mem_id;
struct orders* shared_mem_addr;

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

void get_semaphore_id();
void get_shared_mem_addr ();

int take_semaphore();
void return_semaphore();


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    atexit(at_exit);
    register_sigint_handler();
 
    get_semaphore_id();
    get_shared_mem_addr();

    while(1) {
        usleep((rand()%2000+500)*1000);

        if (semctl(semaphore_id, SENDER_SEM, GETVAL, NULL) > 0) {
            take_semaphore();

            int package_size = 3*shared_mem_addr->orders[shared_mem_addr->sender_index];
            int to_pack_no = semctl(semaphore_id, PACKER_SEM, GETVAL, NULL);
            int to_send_no = semctl(semaphore_id, SENDER_SEM, GETVAL, NULL);

            shared_mem_addr->orders[shared_mem_addr->sender_index] *= 3;
            shared_mem_addr->sender_index = (shared_mem_addr->sender_index+1)%MAX_ORDERS_NO;

            struct timespec tp;
            clock_gettime(CLOCK_REALTIME, &tp);
            printf("(%d, %ld s %ld ms %ld us) Wysłałem zamówienie o wielkości: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), tp.tv_sec, tp.tv_nsec/1000000, (tp.tv_nsec/1000)%1000, package_size, to_pack_no, to_send_no);

            return_semaphore();        
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
    exit(0);
}

void at_exit() {
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmdt(shared_mem_addr);
    printf("Exiting sender.\n");
}

void get_semaphore_id() {
    key_t semaphore_key = ftok(getenv("HOME"), SEM_KEY_ID);

    semaphore_id = semget(semaphore_key, 1, 0);

    if (semaphore_id == -1) {
        printf("Error in obtaining semaphore in receiver. Exiting.\n");
        exit(1);
    }
}

void get_shared_mem_addr () {
    key_t shm_key = ftok(getenv("HOME"), MEM_KEY_ID);

    shared_mem_id = shmget(shm_key, 0, 0);

    if (shared_mem_id < 0) {
        printf("Error in obtaining shared memory id in receiver. Exiting.\n");
        exit(1);
    }

    shared_mem_addr = (struct orders*)shmat(shared_mem_id, NULL, 0);

    if ((void*)shared_mem_addr < 0 ) {
        printf("Error in ataching memory segment in receiver. Exiting.\n");
        exit(1);
    }
}

int take_semaphore() {
    struct sembuf semop_struct[3];
    semop_struct[0].sem_num = ORDERS_SEM;
    semop_struct[0].sem_op = 0;
    
    semop_struct[1].sem_num = ORDERS_SEM;
    semop_struct[1].sem_op = 1;

    semop_struct[2].sem_num = SENDER_SEM;
    semop_struct[2].sem_op = -1;
        
    return semop(semaphore_id, semop_struct, 3);
}

void return_semaphore() {
    struct sembuf semop_struct[1];

    semop_struct[0].sem_num = ORDERS_SEM;
    semop_struct[0].sem_op = -1;
    semop_struct[0].sem_flg = 0;

    semop(semaphore_id, semop_struct, 1);
}
