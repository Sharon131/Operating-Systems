#include "common.h"

pid_t* children_pids;

void* shared_memory_addr;
int sem_id;                 // ?
unsigned short sem_num;     // ?

void at_exit(void){
    //zwolnienie wspólnej pamięci - shmdt
    //shmdt(shared_memory_addr);
    //zwolnienie semafora do wspólnej pamięci - semctl
    //semctl(sem_id, sem_num,IPC_RMID);
    //wysłanie sygnału kill do wszystkich dzieciorów?

    free(children_pids);
}

void create_shared_memory_semaphore() {
    key_t semaphore_key = ftok(getenv("HOME"), SEM_KEY_ID);

    int semaphore_id = semget(semaphore_key, 1, IPC_CREAT);
}

void create_shared_memory_segment() {
    ;
}

int main(int argc, char** argv) {

    create_shared_memory_semaphore();
    create_shared_memory_segment();

    int receivers_no = 0;       // ? atoi czy rand ?
    int packers_no = 0;
    int senders_no = 0;
    int children_no = receivers_no + packers_no + senders_no;

    children_pids = calloc(children_no, sizeof(pid_t));

    int indx = 0;
    children_pids[indx++] = fork();

    while(children_pids[indx-1]!= 0 && indx<children_no) {
        children_pids[indx] = fork();
        indx++;
    }

    if(children_pids[indx-1]==0) {
        if (indx < receivers_no) {
            //exec receiver
        } else if (indx < receivers_no + packers_no) {
            // exec packer
        } else {
            // exec sender
        }
    } else {

        while (children_no > 0) {
            wait(NULL);
            children_no--;
        }
    }

    return 0;
}