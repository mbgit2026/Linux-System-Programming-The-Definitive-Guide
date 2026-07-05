#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s -o|-d", argv[0]);
        fprintf(stderr, "\n -o Obfuscate");
        fprintf(stderr, "\n -d De-obfuscate");
        return 1;
    }

    char data[128];
    size_t len = 0;

    if (strcmp(argv[1], "-o") == 0) {

        printf("Insert the data: ");
        fgets(data, sizeof(data), stdin);
        len = strlen(data);
        memfrob(data, strlen(data));

        printf("Obfuscated: ");
        for (size_t i = 0; i < len; i++)
            printf("%02x ", (unsigned char)data[i]);
        printf("\n");

    } else if (strcmp(argv[1], "-d") == 0) {

        unsigned char buf[6];
        size_t n = fread(buf, 1, 6, stdin);
        if (n != 6) {
            fprintf(stderr, "Expected 6 bytes, got %zu\n", n);
            return 1;
        }

        memfrob(buf, 6);

        printf("Recovered: ");
        fwrite(buf, 1, 6, stdout);
        putchar('\n');
    } else {
        fprintf(stderr, "\nInvalid option");
        return 1;
    }

    return 0;
}
