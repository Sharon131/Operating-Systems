#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "../zad2/biblioteka.h"

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Za mało argumentów.\n");
		exit(-1);
	}
	if (argc < 3) {
		printf("Za mało argumentów\n");
		exit(-1);
	}

	int mt_size = atoi(argv[1]);
	time_t sys_time;
	time_t real_time;
	time_t user_time;

	if (strcmp(argv[2], "create_table")==0) {
		main_table mt;
		MT_createTable(&mt, mt_size);
	}

	if (strcmp(argv[2], "compare_pairs")==0) {
		
		for (int i=3;i<argc;i++) {
			;
		}

	}


	real_time = user_time/CLOCKS_PER_SEC;

}


