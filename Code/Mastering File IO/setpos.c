#include <stdio.h>

int main (int argc, char *argv[])
{
	FILE *fp;
	fpos_t pos;

	fp = fopen ("./file.txt", "w+");
	fgetpos (fp, &pos);
	fputs ("Hello World", fp);
	fflush(fp);

	char c;
	printf("\nString written press a button to continue");
	scanf("%c", &c);

	fsetpos (fp, &pos);
	fputs ("Linux", fp);
	fclose (fp);

	return 0;
}
