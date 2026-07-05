#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <sys/stat.h>

int main(void) {
    setlocale(LC_ALL, "");

    const char *fifo = "/tmp/wide_fifo";

    // Create FIFO if it doesn't exist
    mkfifo(fifo, 0666);

    FILE *fp = fopen(fifo, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fwide(fp, 1); // wide-oriented

    wprintf(L"Type wide characters. Ctrl-D to quit.\n");

    wint_t wc;
    while ((wc = getwchar()) != WEOF) {
        fputwc(wc, fp);
        fflush(fp); // force sending to consumer
    }

    fclose(fp);
    return 0;
}
