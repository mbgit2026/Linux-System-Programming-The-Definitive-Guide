#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/* Key for the thread-specific buffer */
static pthread_key_t buffer_key;

/* Once-only initialisation of the key */
static pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

/* Forward declarations */
static void buffer_key_alloc();
static void buffer_destroy(void *buf);

/* Allocate the thread-specific buffer */
void buffer_alloc(void)
{
    pthread_once(&buffer_key_once, buffer_key_alloc);
    pthread_setspecific(buffer_key, malloc(100));
}

/* Return the thread-specific buffer */
char *get_buffer(void)
{
    return (char *) pthread_getspecific(buffer_key);
}

/* Allocate the key */
static void buffer_key_alloc()
{
    pthread_key_create(&buffer_key, buffer_destroy);
}

/* Free the thread-specific buffer */
static void buffer_destroy(void *buf)
{
    free(buf);
}

/* Thread function */
void *thread_func(void *arg)
{
    int thread_id = *(int *)arg;

    /* Allocate thread-specific buffer */
    buffer_alloc();

    char *buf = get_buffer();

    sprintf(buf, "Hello from thread %d", thread_id);

    printf("%s\n", buf);

    return NULL;
}

int main()
{
    pthread_t threads[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++)
    {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}