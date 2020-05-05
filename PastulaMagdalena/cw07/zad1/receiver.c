#include "common.h"

#include <time.h>

void at_exit(void) {
    ;
}



int main(int argc, char** argv) {
    srand(time(NULL));
    // retrieve key and id for semaphore and shared memory ?
    
    while (1) {
        // losowanie wielkości nowej paczki
        int package_size = rand();
        // umieszczenie tej wielkości w pamięci współdzielonej
        ;
        // wypisanie na ekran informacji po doadniu do pamięci wspólnej:
        // tylko w przypadku udanego wpisania (może być nieudane ?)
        // dodać timestamp -> czas aktualny co do milisek 
        // oraz uaktualnić liczbę m i x, aktualnie zera.
        printf("%d Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.", getpid(), package_size, 0, 0);
    }


    return 0;
}