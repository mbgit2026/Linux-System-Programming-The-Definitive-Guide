#include <sys/inotify.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

void displayEvent(struct inotify_event *ev) {
    printf("\n------------------------------------------------");
    printf("\nEVENT:\t wd=%d cookie=%d ", ev->wd, ev->cookie);
    printf("\nMASK: ");
    if (ev->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (ev->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (ev->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (ev->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (ev->mask & IN_CREATE) printf("IN_CREATE ");
    if (ev->mask & IN_DELETE) printf("IN_DELETE ");
    if (ev->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (ev->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (ev->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (ev->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (ev->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (ev->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (ev->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (ev->mask & IN_OPEN) printf("IN_OPEN ");
    if (ev->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (ev->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");

    if (ev->len > 0) printf("\nName: %s", ev->name);
}


int main (int argc, char *argv[]){

    if (argc != 2) {
        printf("\nUsage: $0 <pathname>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd, wd, bread;
    int BUFLEN = sizeof(struct inotify_event) + NAME_MAX;
    char buf[BUFLEN] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    int i = 0;
    struct inotify_event *event;


    fd = inotify_init();

    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, argv[1], IN_ALL_EVENTS | IN_EXCL_UNLINK);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        bread = read(fd, buf, BUFLEN);

        if (bread == 0) 
            printf("\nNo bytes read from inotify descriptor");
        else if (bread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
     
        i = 0;
        while (i < bread) {
            event = (struct inotify_event *) &buf[i];
            displayEvent(event);
            i += sizeof(struct inotify_event) + event->len;
        }
    
    }
    
    exit(EXIT_SUCCESS);
}