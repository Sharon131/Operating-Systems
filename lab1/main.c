#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {

	int som = system("diff test_a.txt test_b.txt > diff_output.txt");
	char* diff_file = "diff_output.txt";
	
	int fd = open(diff_file, O_RDONLY);
	printf("Return code form system: %d\n", som);

	char* buff = calloc(100, sizeof(char));
	read(fd, buff, 100);

	printf("Zawartosc pliku diff: %s\n", buff);

	close(fd);
	free(buff);
}


