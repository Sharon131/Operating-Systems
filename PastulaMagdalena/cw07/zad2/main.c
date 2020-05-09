#include "common.h"

#include <errno.h>

int children_no;
pid_t* children_pids;

sem_t* semaphore_addr;
int shared_mem_des;

void* shared_mem_addr;

void register_sigint_handler ();
void handle_sigint(int sig_num);

void at_exit(void);

void create_semaphore();
void create_shared_memory_segment();


/*--------------------------------------------------------------*/


int main(int argc, char** argv) {

    atexit(at_exit);
    register_sigint_handler();

    create_semaphore();
    create_shared_memory_segment();

    int receivers_no = rand()%3+1;
    int packers_no = rand()%3+1;
    int senders_no = rand()%3+1;
    children_no = receivers_no + packers_no + senders_no;

    printf("No receivers: %d\n", receivers_no);
    printf("No packers: %d\n", packers_no);
    printf("No senders: %d\n", senders_no);

    children_pids = calloc(children_no, sizeof(pid_t));

    int indx = 0;
    children_pids[indx++] = fork();

    while(children_pids[indx-1]!= 0 && indx<children_no) {
        children_pids[indx] = fork();
        indx++;
    }

    if(children_pids[indx-1]==0) {
        if (indx-1 < receivers_no) {
            execlp("./receiver", "0", NULL);
        } else if (indx-1 < receivers_no + packers_no) {
            execlp("./packer", "0", NULL);
        } else {
            execlp("./sender", "0", NULL);
        }
    } else {

        while (children_no > 0) {
            wait(NULL);
            children_no--;
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

void at_exit(void){    

    sem_close(semaphore_addr);
    munmap(shared_mem_addr, sizeof(struct orders));

    for (int i=0;i<children_no;i++) {
        kill(children_pids[i], SIGINT);
    }

    free(children_pids);

    sem_unlink(SEM_NAME);
    shm_unlink(MEM_NAME);

    printf("Exiting main.\n");
}

void create_semaphore() {
    
    semaphore_addr = sem_open(SEM_NAME, O_CREAT | O_EXCL,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1);

    if (semaphore_addr == SEM_FAILED) {
        printf("Error in creating semaphore. Exiting.\n");
        exit(1);
    }
}

void create_shared_memory_segment() {
    
    shared_mem_des = shm_open(MEM_NAME, O_CREAT | O_EXCL | O_RDWR,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (shared_mem_des == -1) {
        printf("Error in creating shared memory segment. Exiting.\n");
        exit(1);
    }

    if (ftruncate(shared_mem_des, (off_t)sizeof(struct orders)) == -1) {
        printf("Error in truncating shared memory. Exiting.\n");
        exit(1);
    }

    shared_mem_addr = mmap(NULL, sizeof(struct orders), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shared_mem_des, 0);

    if (shared_mem_addr < 0 ) {
        printf("Error in ataching memory segment. Exiting.\n");
        exit(1);
    }
    
    ((struct orders*)shared_mem_addr)->receiver_index = 0;
    ((struct orders*)shared_mem_addr)->sender_index = 0;
    ((struct orders*)shared_mem_addr)->packer_index = 0;
    ((struct orders*)shared_mem_addr)->to_pack_no = 0;
    ((struct orders*)shared_mem_addr)->to_send_no = 0;
}