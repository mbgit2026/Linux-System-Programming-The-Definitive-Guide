#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {

    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "%s <filename> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
		perror("lstat");
		exit (EXIT_FAILURE);
	}

    int type = sb.st_mode & S_IFMT;
	
    if(S_ISDIR(type)) {
        if (unlinkat(AT_FDCWD, argv[1], AT_REMOVEDIR) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    } else {
        if (unlink(argv[1]) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    }


    exit(EXIT_SUCCESS);

}