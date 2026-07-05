#include <stdio.h>

int main (int argc, char *argv[])
{
	char command[128];
	FILE *fp;

	for (;;) {
		fputs("command> ", stdout);

		fgets(command, 64, stdin);
		fp = popen (command, "w");
		fputs(command, fp);
	        pclose (fp);
		putchar ('\n');
	}

	return 0;
}	


