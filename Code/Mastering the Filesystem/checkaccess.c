#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[]) 
{
	struct stat sb;
	
	if (argc != 2) {
		printf("\nUsage: %s <file>", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if (lstat(argv[1], &sb) == -1) {
		perror("lstat");
		exit (EXIT_FAILURE);
	}
	
	int type = sb.st_mode & S_IFMT;
	
	if (access(argv[1], R_OK) == 0)
		printf("\n%s read access OK", argv[1]);
	else
		perror("access");
	if (access(argv[1], W_OK) == 0)
		printf("\n%s write access OK", argv[1]);
	else
		perror("access");
	if (access(argv[1], X_OK) == 0)
		printf("\n%s execute access OK", argv[1]);
	else
		perror("access");
	
		
	if(S_ISLNK(type)) { 
		if (faccessat(AT_FDCWD, argv[1], R_OK, AT_SYMLINK_NOFOLLOW) == 0)
			printf("\nSymbolic link %s read access OK", argv[1]);
		if (faccessat(AT_FDCWD, argv[1], W_OK, AT_SYMLINK_NOFOLLOW) == 0)
			printf("\nSymbolic link %s write access OK", argv[1]);
		if (faccessat(AT_FDCWD, argv[1], X_OK, AT_SYMLINK_NOFOLLOW) == 0)
			printf("\nSymbolic link %s execute access OK", argv[1]);
	}

	printf("\nReal UID: %d: ", getuid());	
	printf("\nEffective UID: %d: ", geteuid());

	return 0;
}
	
