#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
 	if (argc != 2) {
 		printf("\nUsage: %s <dirname>", argv[0]);
		exit(EXIT_FAILURE);
 	}
 	if ((mkdir(argv[1], S_IRWXU | S_IRGRP | S_IXGRP)) == -1) {
 		perror("mkdir");
 		exit(EXIT_FAILURE);
 	}
 	exit(EXIT_SUCCESS);
}