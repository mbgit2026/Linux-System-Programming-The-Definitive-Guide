#include <stdio.h>

int main() 
{
    char c;
    FILE *fp;

    fp = fopen("./file.txt", "r");
    while ((c=fgetc(fp)) != EOF)
        fputc(c, stdout);

    fclose(fp);
    return 0;
}
