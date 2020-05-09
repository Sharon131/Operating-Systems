#include "common.h"


int children_no;
pid_t* children_pids;

int shared_mem_id;
int semaphore_id;
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
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmdt(shared_mem_addr);
    
    for (int i=0;i<children_no;i++) {
        kill(children_pids[i], SIGINT);
    }

    shmctl(shared_mem_id, IPC_RMID, NULL);
    free(children_pids);

    printf("Exiting main.\n");
}

void create_semaphore() {
    key_t semaphore_key = ftok(getenv("HOME"), SEM_KEY_ID);

    semaphore_id = semget(semaphore_key, SEMAPHORES_NO, IPC_CREAT | IPC_EXCL | 0666);

    if (semaphore_id == -1) {
        printf("Error in creating semaphore. Exiting.\n");
        exit(1);
    }

    union semun arg;
    arg.val = 0;
    semctl(semaphore_id, ORDERS_SEM, SETVAL, arg);
    semctl(semaphore_id, PACKER_SEM, SETVAL, arg);
    semctl(semaphore_id, SENDER_SEM, SETVAL, arg);
}

void create_shared_memory_segment() {
    key_t shared_mem_key = ftok(getenv("HOME"), MEM_KEY_ID);

    shared_mem_id = shmget(shared_mem_key, sizeof(struct orders), IPC_CREAT | IPC_EXCL | 0666);

    if (shared_mem_id < 0) {
        printf("Error in creating shared memory id. Exiting.\n");
        exit(1);
    }

    shared_mem_addr = shmat(shared_mem_id, NULL, 0);

    if (shared_mem_addr < 0 ) {
        printf("Error in ataching memory segment. Exiting.\n");
        exit(1);
    }
    
    ((struct orders*)shared_mem_addr)->receiver_index = 0;
    ((struct orders*)shared_mem_addr)->sender_index = 0;
    ((struct orders*)shared_mem_addr)->packer_index = 0;
}