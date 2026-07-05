#define _GNU_SOURCE
#include <stdio.h>
#include <wchar.h>

int main(void) {
    FILE *fp1 = tmpfile(); 
    FILE *fp2 = tmpfile(); 

    printf("=== Initial orientations ===\n");
    printf("fp1: %d (0 = not set)\n", fwide(fp1, 0));
    printf("fp2: %d (0 = not set)\n", fwide(fp2, 0));

    int r1 = fwide(fp1, 1);
    printf("\nRequest wide on fp1, fwide returned: %d (>0 = wide)\n", r1);

    int r2 = fwide(fp2, -1);
    printf("Request byte on fp2, fwide returned: %d (<0 = byte)\n", r2);

    fputwc(L'Ω', fp1);
    printf("\nAfter writing wide char to fp1, orientation: %d\n", fwide(fp1,0));

    fputc('A', fp2);
    printf("After writing byte char to fp2, orientation: %d\n", fwide(fp2,0));

    fclose(fp1);
    fclose(fp2);

    return 0;
}

