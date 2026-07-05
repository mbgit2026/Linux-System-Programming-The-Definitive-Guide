#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/fanotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/limits.h>

#define EVENT_BUF_SIZE 4096

void print_event_mask(uint64_t mask) {
    printf(" [");
    if (mask & FAN_ACCESS)        printf("ACCESS ");
    if (mask & FAN_MODIFY)        printf("MODIFY ");
    if (mask & FAN_CLOSE_WRITE)   printf("CLOSE_WRITE ");
    if (mask & FAN_CLOSE_NOWRITE) printf("CLOSE_NOWRITE ");
    if (mask & FAN_OPEN)          printf("OPEN ");
    if (mask & FAN_OPEN_EXEC)     printf("OPEN_EXEC ");
    if (mask & FAN_EVENT_ON_CHILD) printf("ON_CHILD ");
    if (mask & FAN_CREATE)        printf("CREATE ");
    if (mask & FAN_DELETE)        printf("DELETE ");
    if (mask & FAN_DELETE_SELF)   printf("DELETE_SELF ");
    if (mask & FAN_MOVE_SELF)     printf("MOVE_SELF ");
    if (mask & FAN_MOVED_FROM)    printf("MOVED_FROM ");
    if (mask & FAN_MOVED_TO)      printf("MOVED_TO ");
    if (mask & FAN_ATTRIB)        printf("ATTRIB ");
    printf("]");
}

void handle_event(int fd) {
    char path[PATH_MAX];
    struct fanotify_event_metadata buffer[EVENT_BUF_SIZE];

    ssize_t len = read(fd, buffer, sizeof(buffer));
    if (len < 0) {
        perror("read");
        return;
    }

    struct fanotify_event_metadata *metadata = buffer;
    while (FAN_EVENT_OK(metadata, len)) {
        if (metadata->fd >= 0) {
            snprintf(path, sizeof(path), "/proc/self/fd/%d", metadata->fd);
            char actual_path[PATH_MAX];
            ssize_t path_len = readlink(path, actual_path, sizeof(actual_path) - 1);
            if (path_len >= 0) {
                actual_path[path_len] = '\0';
                print_event_mask(metadata->mask);
                printf(" %s\n", actual_path);
            }

            close(metadata->fd);
        }
        metadata = FAN_EVENT_NEXT(metadata, len);
    }
}

int main(int argc, char *argv[]) {
   /* if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }*/

    const char *path = argv[1];

    int fan_fd = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY | O_CLOEXEC);
    if (fan_fd < 0) {
        perror("fanotify_init");
        return 1;
    } 

    if (argv[1]) {
        if (fanotify_mark(fan_fd, FAN_MARK_ADD, FAN_ALL_EVENTS | FAN_EVENT_ON_CHILD, AT_FDCWD, path) < 0) {
            perror("fanotify_mark");
            close(fan_fd);
            exit(EXIT_FAILURE);
        } 
    } else {
        if (fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_ALL_EVENTS, AT_FDCWD, "/") < 0) {
	        perror("fanotify_mark");
	        close(fan_fd);
	        exit(EXIT_FAILURE);
} 
    }

    printf("Monitoring events on: %s\n", path);
    struct pollfd fds[1];
    fds[0].fd = fan_fd;
    fds[0].events = POLLIN;

    while (1) {
        int poll_num = poll(fds, 1, -1);
        if (poll_num == -1) {
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            handle_event(fan_fd);
        }
    }

    close(fan_fd);
    return 0;
}
