#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	
	char *pathfile;
	mode_t mode = S_IRUSR | S_IXUSR;

	if (argc < 2) {
		printf("\nUsage: changeperm <pathfile>");
		return 1;
	}

	chmod(argv[1], mode);

	return 0;
}
	
