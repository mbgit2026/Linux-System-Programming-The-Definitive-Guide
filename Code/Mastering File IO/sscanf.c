#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *str1 = "42";
    const char *str2 = "3.14159";
    const char *str3 = "Hello 123 World";
    const char *str4 = "FF";
    const char *str5 = "Alice:25 Bob:30";

    int i;
    double d;
    char word1[32], word2[32];
    unsigned int hex;
    char name1[32], name2[32];
    int age1, age2;

    sscanf(str1, "%d", &i);
    printf("Input: \"%s\" → integer = %d\n", str1, i);

    sscanf(str2, "%lf", &d);
    printf("Input: \"%s\" → double = %f\n", str2, d);

    sscanf(str3, "%s %d %s", word1, &i, word2);
    printf("Input: \"%s\" → word1 = %s, int = %d, word2 = %s\n", str3, word1, i, word2);

    sscanf(str4, "%x", &hex);
    printf("Input: \"%s\" → hex = %u (decimal)\n", str4, hex);

    sscanf(str5, "%[^:]:%d %[^:]:%d", name1, &age1, name2, &age2);
    printf("Input: \"%s\" → %s is %d, %s is %d\n", str5, name1, age1, name2, age2);

    return 0;
}
