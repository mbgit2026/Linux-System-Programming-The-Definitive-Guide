#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <gnu/lib-names.h>  /* Defines LIBM_SO */

typedef double (*cos_fn_t)(double);

int main(void)
{
    void *handle;
    cos_fn_t cosine;
    void *sym;
    char *error;

    handle = dlopen(LIBM_SO, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror();    /* Clear any existing error */

    sym = dlsym(handle, "cos");

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    /* POSIX-compliant assignment without casting */
    *(void **)(&cosine) = sym;

    printf("%f\n", cosine(2.0));

    dlclose(handle);
    return EXIT_SUCCESS;
}

