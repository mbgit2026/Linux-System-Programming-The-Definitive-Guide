#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "");

    FILE *fp = fopen("file.txt", "w+");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fputwc(L'Á', fp);
    fputwc(L'Ω', fp);
    fputwc(L'你', fp);
    fputwc(L'\n', fp);

    rewind(fp);

    wint_t wc;
    while ((wc = fgetwc(fp)) != WEOF) {
        wprintf(L"Read: %lc (U+%04X)\n", wc, wc);
    }

    if (feof(fp)) {
        wprintf(L"Reached end of file (WEOF)\n");
    } else if (ferror(fp)) {
        wprintf(L"Error while reading\n");
    }

    fclose(fp);
    return 0;
}
