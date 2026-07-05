#include <stdio.h>
#include <unistd.h>   

int main(void) {

    printf("=== FULL BUFFERING ===\n");
    setvbuf(stdout, NULL, _IOFBF, 0);
    fputs("Hello (FULL)", stdout);
    sleep(2);  
    fflush(stdout); 
    sleep(1);

    printf("\n=== LINE BUFFERING ===\n");
    setvbuf(stdout, NULL, _IOLBF, 0);
    fputs("Hello (LINE, no newline)", stdout);
    sleep(2); 
    fputs("\nHello (LINE, with newline)\n", stdout);
    sleep(1);

    printf("\n=== NO BUFFERING ===\n");
    setvbuf(stdout, NULL, _IONBF, 0);
    fputs("Hello (NO)", stdout);
    sleep(2); 
    fputs(" World (NO)\n", stdout);
    sleep(1);

    return 0;
}
