#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <string>", argv[0]);
        exit(EXIT_FAILURE);
    }

    setlocale(LC_ALL, "");
    const char *mb_str = argv[1];  // multibyte string (ASCII)
    wchar_t wc_str[100];
    char mb_str_back[100];

    size_t n1 = mbstowcs(wc_str, mb_str, 100);
    if (n1 == (size_t)-1) {
        perror("mbstowcs");
        exit(EXIT_FAILURE);
    }

    printf("Multibyte string: %s\n\n", mb_str);
    printf("Converted to wide characters:\n");
    for (size_t i = 0; i < n1; i++) {
        printf("  wc_str[%zu] = U+%04X\n", i, wc_str[i]);
    }

    size_t n2 = wcstombs(mb_str_back, wc_str, 100);
    if (n2 == (size_t)-1) {
        perror("wcstombs");
        exit(EXIT_FAILURE);
    }

    printf("\nBack to multibyte string: %s\n", mb_str_back);
    exit(EXIT_SUCCESS);
}
