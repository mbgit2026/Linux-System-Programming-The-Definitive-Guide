#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handle_sigabrt(int sig) {
    printf("Caught signal %d SIGABRT\n", sig);
    
    exit(1);  
}

int main(void) {
    signal(SIGABRT, handle_sigabrt);

    printf("Calling abort()...\n");

    abort();  

    
    printf("This will not be printed.\n");
    return 0;
}
