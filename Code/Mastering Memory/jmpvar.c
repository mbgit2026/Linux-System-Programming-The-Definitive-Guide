#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

void jump_back(void)
{
    longjmp(env, 1);
}

int main(void)
{
    int x = 10;        
    
    if (setjmp(env) == 0) {
        printf("Before longjmp: x = %d\n", x);
        x = 42;        
        jump_back();
    } else {
        printf("After longjmp:  x = %d\n", x);
    }

    return 0;
}
