#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char input[128];
    char *first_name = NULL;
    char *last_name = NULL;
    int age;

    printf("Enter your first name, last name, and age: ");
    if (!fgets(input, sizeof(input), stdin)) {
        perror("fgets");
        return 1;
    }

    sscanf(input, "%ms %ms %d", &first_name, &last_name, &age);

    printf("First name: %s\n", first_name);
    printf("Last name:  %s\n", last_name);
    printf("Age:        %d\n", age);

    free(first_name);
    free(last_name);

    return 0;
}
