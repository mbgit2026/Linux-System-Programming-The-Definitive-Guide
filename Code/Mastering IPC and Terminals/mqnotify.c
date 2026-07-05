#include <err.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void tfunc(union sigval sv)                     /* Thread start function */
{
    struct mq_attr attr;
    ssize_t nr;
    void *buf;
    mqd_t mqdes = *((mqd_t *) sv.sival_ptr);

    /* Determine max. msg size; allocate buffer to receive msg */

    if (mq_getattr(mqdes, &attr) == -1)
        err(EXIT_FAILURE, "mq_getattr");
    buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
        err(EXIT_FAILURE, "malloc");

    nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
    if (nr == -1)
        err(EXIT_FAILURE, "mq_receive");

    printf("Read %zd bytes from MQ\n", nr);
    free(buf);
    exit(EXIT_SUCCESS);         /* Terminate the process */
}

int main(int argc, char *argv[])
{
    mqd_t mqdes;
    struct sigevent sev;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mq-name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqdes = mq_open(argv[1], O_RDONLY);
    if (mqdes == (mqd_t) -1)
        err(EXIT_FAILURE, "mq_open");

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = tfunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &mqdes;   /* Arg. to thread func.  */
  
    if (mq_notify(mqdes, &sev) == -1)
        err(EXIT_FAILURE, "mq_notify");

    pause();    /* Process will be terminated by thread function */
}
