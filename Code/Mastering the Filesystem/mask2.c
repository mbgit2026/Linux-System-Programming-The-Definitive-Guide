
#include <sys/stat.h>
#include <fcntl.h>

#define FILEPERM (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
#define NOPERM (S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)


int main() 
{

	umask(0);
	int f = open("newfile.txt", O_CREAT | O_RDWR | O_EXCL, FILEPERM);	

	umask(NOPERM);
	int f2 = open("newfile2.txt", O_CREAT | O_RDWR | O_EXCL, FILEPERM);
	

	return 0;
}
