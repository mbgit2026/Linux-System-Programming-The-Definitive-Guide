#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *fp;

	if (argc != 2) {
		printf ("Usage: %s <file>", argv[0]);
		return 1;
	}

	if ((fp = fopen (argv[1], "r")) == NULL) {
		printf ("%s: %s", argv[0], strerror (errno));
		return 1;
	} else 
		return 0;
		
}


