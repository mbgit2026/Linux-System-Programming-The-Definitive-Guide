#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

  if (argc < 2) {
    printf("\nUsage: %s <filename>", argv[0]);
    return 1;
  }

  int fd;
  fd = open (argv[1], O_WRONLY);
  
  if (fd != -1) {
    printf ("\nPID %d, the file %s already exists", getpid(), argv[1]);
    close (fd);
    exit(EXIT_FAILURE);
  } 

  if (errno != ENOENT) 
    exit(EXIT_FAILURE);


  printf ("\nPID %d, file %s does not exists", getpid(), argv[1]); 

  if (argc > 2) {
    sleep (15);
    printf ("\nPID %d, end of sleep \n", getpid());
  } 

  fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);	
  }

  printf ("\nPID %d: file %s created exclusively\n", getpid(), argv[1]);
  return 0;
}

