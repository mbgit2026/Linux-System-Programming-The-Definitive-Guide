#define _GNU_SOURCE
#include <stdio.h>
#include <wchar.h>
#include <unistd.h>

int main(void) {
    FILE *in  = stdin;
    FILE *out = stdout;

    printf("=== Before any I/O ===\n");
    printf("stdin orientation:  %d (0 = undecided)\n", fwide(in,0));
    printf("stdout orientation: %d (0 = undecided)\n", fwide(out,0));

    // Attempt to request wide orientation
    int r_in  = fwide(in, 1);
    int r_out = fwide(out, 1);
    printf("\nRequested wide orientation:\n");
    printf("stdin fwide returned:  %d\n", r_in);
    printf("stdout fwide returned: %d\n", r_out);

    // Perform I/O
    printf("\nType a line and press Enter: ");
    fflush(stdout);  // ensure prompt shows

    int c = fgetc(in);  // first input sets stdin orientation if undecided
    ungetc(c, in);      // put it back

    printf("Reading one char from stdin sets orientation...\n");
    printf("stdin orientation now:  %d\n", fwide(in,0));
    printf("stdout orientation now: %d\n", fwide(out,0));

    // Write a wide character to stdout
    fputwc(L'Ω', out);
    fputwc(L'\n', out);
    fflush(out);

    return 0;
}
