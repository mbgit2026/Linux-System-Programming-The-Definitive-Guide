#include <fcntl.h>
#include <unistd.h>

int main ()
{

//  openat (AT_FDCWD, "./file.txt", O_RDONLY);

/* 
  int fd = open ("/home/io/", O_RDONLY);
	openat (fd, "file.txt", O_RDONLY); 
*/ 

openat (0, "/home/io/file.txt", O_RDONLY);

	sleep (50);
}

