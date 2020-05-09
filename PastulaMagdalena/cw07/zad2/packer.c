#include "common.h"

sem_t* semaphore_addr;
int shared_mem_des;         
struct orders* shared_mem_addr;

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

void get_semaphore_addr();
void get_shared_mem_addr ();

int take_semaphore();
void return_semaphore();


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    atexit(at_exit);
    register_sigint_handler();

    get_semaphore_addr();
    get_shared_mem_addr();

    while(1) {
        usleep((rand()%2000+500)*1000);

        if (shared_mem_addr->to_pack_no > 0 && take_semaphore()) {
            
            shared_mem_addr->to_pack_no--;

            int package_size = 2*shared_mem_addr->orders[shared_mem_addr->packer_index];
            int to_pack_no = shared_mem_addr->to_pack_no;
            int to_send_no = shared_mem_addr->to_send_no;

            shared_mem_addr->orders[shared_mem_addr->packer_index] *= 2;
            shared_mem_addr->packer_index = (shared_mem_addr->packer_index+1)%MAX_ORDERS_NO;

            shared_mem_addr->to_send_no++;

            struct timespec tp;
            clock_gettime(CLOCK_REALTIME, &tp);
            printf("(%d, %ld s %ld ms %ld us) Przygotowałem zamówienie o wielkości: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), tp.tv_sec, tp.tv_nsec/1000000, (tp.tv_nsec/1000)%1000, package_size, to_pack_no, to_send_no+1);   

            FILE* shm_fp = fdopen(shared_mem_des, "a");
            fflush(shm_fp);

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
    sem_close(semaphore_addr);
    munmap(shared_mem_addr, sizeof(struct orders));
    
    printf("Exiting packer.\n");
}

void get_semaphore_addr() {
    semaphore_addr = sem_open(SEM_NAME, O_RDWR);

    if (semaphore_addr == SEM_FAILED) {
        printf("Error in obtaining semaphore in receiver. Exiting.\n");
        exit(1);
    }
}

void get_shared_mem_addr () {
    
    shared_mem_des = shm_open(MEM_NAME, O_RDWR, 0);

    if (shared_mem_des < 0) {
        printf("Error in obtaining shared memory id in receiver. Exiting.\n");
        exit(1);
    }

    shared_mem_addr = mmap(NULL, sizeof(struct orders), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_mem_des, 0);

    if ((void*)shared_mem_addr < 0 ) {
        printf("Error in ataching memory segment in receiver. Exiting.\n");
        exit(1);
    }
}

int take_semaphore() {
    return sem_trywait(semaphore_addr);
}

void return_semaphore() {
    sem_post(semaphore_addr);
}