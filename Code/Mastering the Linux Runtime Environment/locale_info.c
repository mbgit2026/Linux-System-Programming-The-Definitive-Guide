#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <LANG>\n", argv[0]);
        printf("\nExample:\n");
        printf("%s en_US.UTF-8\n", argv[0]);
        printf("%s fr_FR.UTF-8\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!setlocale(LC_ALL, argv[1])) {
        perror("setlocale");
        exit(EXIT_FAILURE);
    }

    struct lconv *lc = localeconv();

    printf("Locale settings:\n");
    printf("Decimal point       : '%s'\n", lc->decimal_point);
    printf("Thousands separator : '%s'\n", lc->thousands_sep);
    printf("Currency symbol     : '%s'\n", lc->currency_symbol);
    printf("Positive sign       : '%s'\n", lc->positive_sign);
    printf("Negative sign       : '%s'\n", lc->negative_sign);
    printf("International currency symbol: '%s'\n", lc->int_curr_symbol);

    exit(EXIT_SUCCESS);
}
