#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RWRWRW (S_IRUSR| S_IWUSR| S_IRGRP| S_IWGRP| S_IROTH| S_IWOTH)

int main() 
{
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);	
	creat("newfile.txt", RWRWRW);
	return 0;
}
