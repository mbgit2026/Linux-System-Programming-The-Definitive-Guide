#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc<2) {
		printf("\nUsage: %s <srcfile> <dstfile>", argv[0]);
		return 1;
	}

	FILE* src_file;
	FILE* dst_file;
	char c;

	if ((src_file = fopen (argv[1], "r")) == NULL) {
		perror("fopen");
		return 1;
	}
	
	if ((dst_file = fopen (argv[2], "a+")) == NULL) {
		perror("fopen");
		return 1;
	}
	
	while ((c = fgetc (src_file)) != EOF)
		fputc (c, dst_file);
		
	fclose (src_file);
	fclose (dst_file);
	return 0;
}

