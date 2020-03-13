#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>

#include "biblioteka.h"

int execute_command(char* command, main_table* mt, char** argv) {
	if (strcmp(command, "create_table")==0) {
		main_table mt;
		int mt_size = atoi(argv[0]);
		MT_createTable(&mt, mt_size);
		return 1;
	}
	else if (strcmp(command, "compare_pairs")==0) {
		int pairs_no = atoi(argv[0]);
		
		MT_defineFiles(mt, argv+1, pairs_no*2);
	
		for (int i=0;i<pairs_no;i++) { // jak mierzyć czas osobno porównywania jak i dodawania bloków?
			MT_comparePairFromDefinedFiles(mt, i);
			MT_createOperationUnitFor(mt, i);
		} 
		
		return pairs_no*2+1;
	}
	else if (strcmp(command, "remove_block")==0) {
		int unit_no = atoi(argv[0]);
		MT_deleteUnit(mt, unit_no);
		return 1;
	}
	else if (strcmp(command, "remove_operation")==0) {
		int unit_no = atoi(argv[0]);
		int op_no = atoi(argv[1]);
		MT_deleteOperation(mt, unit_no, op_no);
		return 2;
	}
	else if (strcmp(command, "add_block")==0) {
		int unit_no = argv[0];

		if (unit_no > mt->units_no) {
			printf("Block index to add is bigger than table size.\n");
			exit(-1);
		}

		if (mt->used_units[unit_no]) {
			MT_deleteUnit(mt, unit_no);
		}

		MT_createOperationUnitFor(mt, unit_no+1);

		return 1;
	}
	else if (strcmp(command, "add_and_delete")==0) {
		int units_no = argv[0];
		int steps_no = argv[1];
		
		if(units_no > mt->units_no) {
			printf("Number of block to add and delete bigger than table size.\n");
			exit(-1);
		}

		for (int i=0;i<units_no;i++) {		
			if (mt->files_seq[i] = NULL) {
				mt->files_seq[i] = "test_a.txt test_b.txt";
			}	
		}

		for (int i=0;i<steps_no;i++) {
			MT_createOperationUnitFor(mt, 0);
			MT_deleteUnit(mt, 0);
		}
		return 2;
	}
	else {
		printf("Not known command.\n");
		printf("Command: %s \n", command);
		exit(-1);
		return 0;
	}
}

void printfTimes(clock_t r_time_start, clock_t r_time_end, struct tms* su_time_start, struct tms* su_time_end) {
	printf("Real time: %f s\n", ((double)r_time_end-r_time_start)/sysconf(_SC_CLK_TCK));
	printf("User time: %f s\n", ((double)su_time_end->tms_utime-su_time_start->tms_utime)/sysconf(_SC_CLK_TCK));
	printf("System time: %f s\n", ((double)su_time_end->tms_stime-su_time_start->tms_stime)/sysconf(_SC_CLK_TCK));
}

bool checkArgs(int argc, char** argv) {
	if (argc == 1) {
		printf("No arguments.\n");
		return false;
	}
	if (argc < 2) {
		printf("Not enough arguments.\n");
		return false;
	}
	if (atoi(argv[1]) <= 0) {
		printf("Table size should be bigger than 0.\n");
		return false;
	}

	return true;
}

int main(int argc, char **argv) {

	if (!checkArgs(argc, argv))		exit(-1);

	int mt_size = atoi(argv[1]);

	struct tms sys_us_time_start;
	clock_t real_time_start = times(&sys_us_time_start);
	
	main_table mt;
	MT_createTable(&mt, mt_size);

	struct tms sys_us_time_end;
	clock_t real_time_end;
	
	real_time_end = times(&sys_us_time_end);

	printf("Times of table creating:\n");
	printfTimes(real_time_start, real_time_end, &sys_us_time_start, &sys_us_time_end);

	int index = 2;
	while (index < argc) {
		real_time_start = times(&sys_us_time_start);
		char* command = argv[index];
		index++;
		index += execute_command(command, &mt, argv+index);
		real_time_end = times(&sys_us_time_end);

		printf("Times taken for command: %s\n", command);
		printfTimes(real_time_start, real_time_end, &sys_us_time_start, &sys_us_time_end);
	}

	return 0;

}


