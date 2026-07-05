#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

int main (int argc, char *argv[]) {
	int fd, lblocks, i, ret;
	struct stat buf;

	if (argc <2 ) {
		printf("Usare: %s <file>\n", argv[0]);
		return 1;
	}

	fd = open (argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	ret = fstat(fd, &buf);
	if (ret < 0) {
		perror("fstat");
		return 1;
	}

	lblocks = buf.st_blocks;
	printf ("\nBlocchi logici: %d\n", lblocks);

	for (i=0; i<lblocks; i++) {

		int j = i;
		ret = ioctl(fd, FIBMAP, &j);

		if (!j) continue;

		printf("%d %d\n", i, j);
	}

	return  0;
}
		
