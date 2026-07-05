#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

void second(void)
{
    printf("Inside second()\n");
    longjmp(env, 42);   /* Jump back to setjmp */
}

void first(void)
{
    printf("Inside first()\n");
    second();
    printf("This line is never executed\n");
}

int main(void)
{
    int ret = setjmp(env);

    if (ret == 0) {
        /* First time setjmp() is called */
        printf("setjmp returned %d (initial call)\n", ret);
        first();
    } else {
        /* Returned via longjmp() */
        printf("setjmp returned %d (after longjmp)\n", ret);
    }

    return 0;
}
