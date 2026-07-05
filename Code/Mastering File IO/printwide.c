#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "");

    wchar_t wc = L'Ω';     // Greek capital letter omega
    wchar_t wc1 = L'€';    // Euro symbol
    wchar_t wc2 = L'∞';    // Infinitive
    wprintf(L"Wide character: %lc (code point U+%04X)\n", wc, wc);
    wprintf(L"Wide character: %lc (code point U+%04X)\n", wc1, wc1);
    wprintf(L"Wide character: %lc (code point U+%04X)\n", wc2, wc2);

    return 0;
}
