#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "biblioteka.h"
#include <string.h>

int main(int argc, char **argv) {

	if (argc == 0) {
		printf("Brak argumentów\n");
		exit(-1);
	}

	//char** files = 0;

	main_table mt;
	MT_createTable(&mt, 10);

	char* test = calloc(17, sizeof(char));
	char* test_a = "Ala ma";
	char* test_b = "może kota";

	test = strcat(test, test_a);
	test[strlen(test_a)] = ' ';
	test = strcat(test, test_b);

	printf("Zawartość test: %s\n", test);
}


