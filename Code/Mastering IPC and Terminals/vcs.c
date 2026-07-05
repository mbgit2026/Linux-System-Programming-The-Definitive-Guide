#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <unistd.h>

int main(void)
{
    int fd;
    char *device = "/dev/vcsa2";
    char *console = "/dev/tty2";
    struct {unsigned char lines, cols, x, y;} scrn;
    unsigned short s;
    unsigned short mask;
    unsigned char attrib;
    int ch;

    fd = open(console, O_RDWR);
    if (fd < 0) {
        perror(console);
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, VT_GETHIFONTMASK, &mask) < 0) {
        perror("VT_GETHIFONTMASK");
        exit(EXIT_FAILURE);
    }
    (void) close(fd);
    fd = open(device, O_RDWR);
    if (fd < 0) {
        perror(device);
        exit(EXIT_FAILURE);
    }
    (void) read(fd, &scrn, 4);
    (void) lseek(fd, 4 + 2*(scrn.y*scrn.cols + scrn.x), SEEK_SET);
    (void) read(fd, &s, 2);
    ch = s & 0xff;
    if (s & mask)
        ch |= 0x100;
    attrib = ((s & ~mask) >> 8);
    printf("ch=%#03x attrib=%#02x\n", ch, attrib);
    s ^= 0x1000;
    (void) lseek(fd, -2, SEEK_CUR);
    (void) write(fd, &s, 2);
    exit(EXIT_SUCCESS);
}
