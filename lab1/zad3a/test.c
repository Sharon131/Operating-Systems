#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>

#ifndef DLL
#include "biblioteka.h"
#endif

#ifdef DLL
#include <dlfcn.h>
void* dll;
typedef struct operation_unit {
    char** operations;
    int size;

} operation_unit;
typedef struct main_table{
    char** files_seq;
    operation_unit** table;
    bool* used_units;
    int units_no;
} main_table;
#endif

int execute_command(char* command, main_table* mt, char** argv) {
	
	if (strcmp(command, "create_table")==0) {
		#ifdef DLL
	    void (*MT_createTable) (main_table* mt, int size) = dlsym(dll, "MT_createTable");
    	#endif

		main_table mt;
		int mt_size = atoi(argv[0]);
		MT_createTable(&mt, mt_size);
		return 1;
	}
	else if (strcmp(command, "compare_pairs")==0) {
		#ifdef DLL
		void (*MT_defineFiles) (main_table*, char**, int) = dlsym(dll, "MT_defineFiles");
		void (*MT_comparePairFromDefinedFiles) (main_table*, int) = dlsym(dll, "MT_comparePairFromDefinedFiles");
		int (*MT_createOperationUnitForLastPair) (main_table*) = dlsym(dll, "MT_createOperationUnitForLastPair");
		#endif

		int pairs_no = atoi(argv[0]);
		
		MT_defineFiles(mt, argv+1, pairs_no*2);
	
		for (int i=0;i<pairs_no;i++) {
			MT_comparePairFromDefinedFiles(mt, i);
			MT_createOperationUnitForLastPair(mt);
		} 
		
		return pairs_no*2+1;
	}
	else if (strcmp(command, "compare_pairs_ntimes")==0) {
		#ifdef DLL
		void (*MT_defineFiles) (main_table*, char**, int) = dlsym(dll, "MT_defineFiles");
		void (*MT_comparePairFromDefinedFiles) (main_table*, int) = dlsym(dll, "MT_comparePairFromDefinedFiles");
		int (*MT_createOperationUnitForLastPair) (main_table*) = dlsym(dll, "MT_createOperationUnitForLastPair");
		#endif
		
		int pairs_no = atoi(argv[0]);
		int compare_no = atoi(argv[1]);

		MT_defineFiles(mt, argv+2, pairs_no*2);
	
		for (int i=0;i<pairs_no;i++) {
			for (int j=0;j<compare_no;j++) {
				MT_comparePairFromDefinedFiles(mt, i);
				MT_createOperationUnitForLastPair(mt);
			}
		} 
		
		return pairs_no*2+2;
	}
	else if (strcmp(command, "remove_block")==0) {
		#ifdef DLL
		void (*MT_deleteUnit) (main_table*, int) = dlsym(dll, "MT_deleteUnit");
		#endif

		int unit_no = atoi(argv[0]);
		MT_deleteUnit(mt, unit_no);
		return 1;
	}
	else if (strcmp(command, "remove_nfirst_blocks")==0) {
		#ifdef DLL
		void (*MT_deleteUnit) (main_table*, int) = dlsym(dll, "MT_deleteUnit");
		#endif		
		
		int units_no = atoi(argv[0]);

		for (int i=0;i<units_no;i++) {
			MT_deleteUnit(mt, i);
		}

		return 1;
	}
	else if (strcmp(command, "remove_operation")==0) {
		#ifdef DLL
		void (*MT_deleteOperation) (main_table*, int, int) = dlsym(dll, "MT_deleteOperation");
		#endif		
		
		int unit_no = atoi(argv[0]);
		int op_no = atoi(argv[1]);
		MT_deleteOperation(mt, unit_no, op_no);
		return 2;
	}
	else if (strcmp(command, "add_block")==0) {
		#ifdef DLL
		void (*MT_createOperationUnitForLastPair) (main_table*) = dlsym(dll, "MT_createOperationUnitForLastPair");
		#endif		
		
		int units_no = atoi(argv[0]);

		for (int i=0;i<units_no;i++) {
			MT_createOperationUnitForLastPair(mt);
		}

		return 1;
	}
	else if (strcmp(command, "add_and_delete")==0) {
		#ifdef DLL
		int (*MT_createOperationUnitForLastPair) (main_table*) = dlsym(dll, "MT_createOperationUnitForLastPair");
		void (*MT_deleteUnit) (main_table*, int) = dlsym(dll, "MT_deleteUnit");
		#endif		
		
		int units_no = atoi(argv[0]);
		int steps_no = atoi(argv[1]);
		
		if(units_no > mt->units_no) {
			printf("Number of block to add and delete bigger than table size.\n");
			exit(-1);
		}

		int* units_nos = calloc(units_no,sizeof(int));

		for (int i=0;i<steps_no;i++) {
			for (int j=0;j<units_no;j++) {
				units_nos[j] = MT_createOperationUnitForLastPair(mt);
			}

			for(int j=0;j<units_no;j++){
				MT_deleteUnit(mt, units_nos[j]);
			}

		}
		return 2;
	}
	else if (strcmp(command, "get_ops_no")==0) {
		#ifdef DLL
		int (*MT_getOperationsCounter) (main_table*, int) = dlsym(dll, "MT_getOperationsCounter");
		#endif		
		
		int unit_no = atoi(argv[0]);
		int op_no = MT_getOperationsCounter(mt, unit_no);

		printf("Op no for block %d: %d\n", unit_no, op_no);

		return 1;
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
	#ifdef DLL
    dll = dlopen("./libbiblioteka.so", RTLD_LAZY);

    void (*MT_createTable) (main_table* mt, int size) = dlsym(dll, "MT_createTable");
    //void (*MT_deleteTable) (main_table* mt) = dlsym(dll, "MT_deleteTable");
	#endif

	if (!checkArgs(argc, argv))		exit(-1);

	int mt_size = atoi(argv[1]);

	main_table mt;

	struct tms sys_us_time_start;
	clock_t real_time_start;
	struct tms sys_us_time_end;
	clock_t real_time_end;
		
	int index = 2;

	if (strcmp(argv[2], "create_table")==0) {
		index++;

		real_time_start = times(&sys_us_time_start);
		
		MT_createTable(&mt, mt_size);

		real_time_end = times(&sys_us_time_end);

		printf("Times of table creating of size %d:\n", mt_size);
		printfTimes(real_time_start, real_time_end, &sys_us_time_start, &sys_us_time_end);
	} else {
		MT_createTable(&mt, mt_size);
	}

	while (index < argc) {
		struct tms sys_us_time_start_w;
		clock_t real_time_start_w = times(&sys_us_time_start_w);
		char* command = argv[index];
		index++;
		index += execute_command(command, &mt, argv+index);
		
		struct tms sys_us_time_end_w;
		clock_t real_time_end_w = times(&sys_us_time_end_w);

		printf("Times taken for command: %s\n", command);
		printfTimes(real_time_start_w, real_time_end_w, &sys_us_time_start_w, &sys_us_time_end_w);
	}

	//MT_deleteTable(&mt);
	#ifdef DLL
    dlclose(dll);
	#endif

	return 0;
}


