#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    char term[L_ctermid];
    char *tty;

    if (ctermid(term) == NULL) {
        perror("ctermid");
        return 1;
    }

    printf("Controlling terminal (ctermid): %s\n", term);

    tty = ttyname(STDIN_FILENO);
    if (tty == NULL) 
        perror("ttyname (stdin)");
    else 
        printf("Terminal for STDIN (ttyname): %s\n", tty);
    
    return 0;
}