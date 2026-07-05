#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  
    if (argc != 3) {
        printf("\nUsage: %s <file> <long string>", argv[0]);
        return EXIT_FAILURE;
    }

    char *longstring = argv[2];
    off_t newpos;
    FILE *fp = fopen(argv[1], "w+");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    fputs(longstring, fp);

    if (fseeko(fp, 0, SEEK_SET) == -1) {
        perror("fseeko");
        fclose(fp);
        return EXIT_FAILURE;
    }

    off_t pos = ftello(fp);
    if (pos == -1) {
        perror("ftello");
        fclose(fp);
        return EXIT_FAILURE;
    }

    printf("Current position: %lld\n", (long long)pos);
    printf("\nEnter position: ");
    scanf("%d", &newpos);

    if (fseeko(fp, newpos, SEEK_CUR) == -1) {
        perror("fseeko");
        fclose(fp);
        return EXIT_FAILURE;
    }

    pos = ftello(fp);
    printf("\nNew position: %lld\n", (long long)pos);
    char c;
    while ((c = fgetc(fp)) != EOF) {
        fputc(c, stdout);
    }

    if (fseeko(fp, 0, SEEK_END) == -1) {
        perror("fseeko");
        fclose(fp);
        return EXIT_FAILURE;
    }

    pos = ftello(fp);
    printf("\n\nEnd position: %lld (file size)\n", (long long)pos);

    fclose(fp);
    return EXIT_SUCCESS;
}
