#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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
	
	
	if (S_ISREG(type)) 
		printf("\n%s: Regular file", argv[1]);
	else if(S_ISLNK(type)) 
		printf("\n%s: Symbolic link", argv[1]);
	else if(S_ISDIR(type)) 
		printf("\n%s: Directory", argv[1]);
	else if(S_ISCHR(type)) 
		printf("\n%s: Character device", argv[1]);
	else if(S_ISBLK(type)) 
		printf("\n%s: Block device", argv[1]);
	else if(S_ISFIFO(type)) 
		printf("\n%s: FIFO", argv[1]);
	else if(S_ISSOCK(type)) 
		printf("\n%s: Socket", argv[1]);	
		
	return 0;
}
	
