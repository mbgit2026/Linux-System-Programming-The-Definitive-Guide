#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <uniname.h>

int main(void) {
    setlocale(LC_ALL, "");

    const char *fifo = "/tmp/wide_fifo";
    char *name;


    FILE *fp = fopen(fifo, "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fwide(fp, 1); // wide-oriented

    wprintf(L"Consumer ready:\n");

    wint_t wc;
    while ((wc = fgetwc(fp)) != WEOF) {
        if (wc != 10) {
            name = unicode_character_name(wc, name);
            wprintf(L"Received wide char: %lc (code: %d) %s\n", wc, wc, name);
        }
    }

    fclose(fp);
    return 0;
}
