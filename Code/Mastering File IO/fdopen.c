#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage %s <filename>", argv[0]);
		return 1;
	}

    int fd;
    FILE *fp;
    char buffer[32];
	char *filename = argv[1];
	

	if ((fd = open(filename, O_RDONLY) ) == -1)
		perror ("open");
   
	if ((fp = fdopen(fd, "rw")) == NULL)
		perror ("fdopen");

    // Read from fd 
    printf("\nReading from file descriptor..");
    read(fd, buffer, sizeof(buffer));
    printf("\n%s",buffer);
 
    //Read on fp
    printf("\n\nReading from the file pointer..\n");
    fseek(fp, 0, SEEK_SET);
    while (fgets(buffer, 32, fp))
        fputs(buffer, stdout);


    close(fd);
    return 0;

}
