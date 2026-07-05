#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <filename>", argv[0], argv[1]);
        return 1;
    }

    const char *filename = argv[1];
    unsigned char png_magic[8] = {
        0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'
    };
    unsigned char buf[8];

    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fread(buf, 1, 8, f);
    fclose(f);

    if (memcmp(buf, png_magic, 8) == 0) {
        printf("This is a PNG file\n");
    } else {
        printf("Not a PNG file\n");
    }

    return 0;
}
