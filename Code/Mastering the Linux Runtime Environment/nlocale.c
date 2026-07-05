#define _GNU_SOURCE 
#include <stdio.h>
#include <locale.h>
#include <langinfo.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("\nUsage: %s <locale>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *locale_name = argv[1];

    locale_t new_locale = newlocale(LC_ALL_MASK, locale_name, NULL);
    if (new_locale == (locale_t)0) {
        perror("newlocale");
        return 1;
    }

    // Temporarily switch to the new locale
    locale_t old_locale = uselocale(new_locale);


    printf("Locale: %s\n", locale_name);
    printf("Day 1:          %s\n", nl_langinfo(DAY_1));
    printf("Abbrev month 1: %s\n", nl_langinfo(ABMON_1));        
    printf("Currency symbol: %s\n", nl_langinfo(CURRENCY_SYMBOL)); 
    printf("Decimal point: %s\n", nl_langinfo(RADIXCHAR));       

    // Restore the old locale
    uselocale(old_locale);

    // Free the French locale object
    freelocale(new_locale);

    return 0;
}
