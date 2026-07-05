#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *out, *in;
	int v, s;
	size_t size;
	char *ptr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s '<num>...'\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	in = fmemopen(argv[1], strlen(argv[1]), "r");
	if (in == NULL)
		err(EXIT_FAILURE, "fmemopen");

	out = open_memstream(&ptr, &size);
	if (out == NULL)
		err(EXIT_FAILURE, "open_memstream");

	for (;;) {
		s = fscanf(in, "%d", &v);
		if (s <= 0)
			break;

		s = fprintf(out, "%d ", v * v);
		if (s == -1)
			err(EXIT_FAILURE, "fprintf");
	}

	fclose(in);
	fclose(out);

	printf("size = %zu; ptr = %s\n", size, ptr);

	free(ptr);
	exit(EXIT_SUCCESS);
}



