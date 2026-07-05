#include <stdio.h>
#include <utmp.h>
#include <stdlib.h>

int main(void) {
    struct utmp *ut;

    setutent();  
    while ((ut = getutent()) != NULL) {
        if (ut->ut_type == USER_PROCESS) {
            printf("User: %s  Line: %s  Host: %s  Time: %ld\n",
                   ut->ut_user, ut->ut_line, ut->ut_host, ut->ut_tv.tv_sec);
        }
    }
    endutent();
    exit(EXIT_SUCCESS);
}
