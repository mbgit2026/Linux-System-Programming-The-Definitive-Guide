#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) 
{
	int fd;
	struct flock fl;
	int ret, cmd, type;

	fd = open ("file.txt", O_RDWR);

	for (;;) {
		printf("\nGet o Set lock (1. get 2.set): ");
		scanf ("%d", &cmd);
		if (cmd != 1 && cmd != 2) {
			printf("\nInsert a valid command\n");
			continue;
		}
		printf("\n1. Read 2. Write ");
		scanf("%d", &type);
		if (type==1)
			fl.l_type=F_RDLCK;
		else if(type==2)
			fl.l_type=F_WRLCK;
		else {
			printf("\nInsert a valid lock type\n");
			continue;
		}

		if(cmd==1) {
			ret = fcntl(fd, F_GETLK, &fl);
			if (ret == 0) {
				if (fl.l_type == F_UNLCK) 
					printf("\nIt is possible to acquire the lock\n");
				else {
					printf("\nLocked by %d", (long) getpid());
					printf("\nTipe: %d", fl.l_type);
					printf("\nStart = %d End = %d", fl.l_start, fl.l_len);
				}
			} else 
				perror("fcntl");
		} else if (cmd==2) {
			int start, len;
			printf("\nInsert start offset: ");
			scanf("%d", &start);
			printf("Insert length: ");
			scanf("%d", &len);
			fl.l_whence = SEEK_SET;
			fl.l_start = start;
			fl.l_len = len;
			ret = fcntl(fd, F_SETLK, &fl);
			if (ret == 0) {
				printf("\nLock acquired");
				printf("\nTipe: %s", fl.l_type == 0 ? "read" :  "write");
				printf("\nRegion: %d - %d", fl.l_start, fl.l_len);
				printf("\n[PID] %d\n", (long) getpid());
			} else {
				printf("\nIncompatible lock");
				printf("\nAnother process is holding a lock on the same region\n");
			}
		} 
	}
}
