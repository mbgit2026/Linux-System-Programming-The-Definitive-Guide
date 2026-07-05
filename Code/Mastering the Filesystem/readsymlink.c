#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>

       int
       main(int argc, char *argv[])
       {
           char         *buf;
           ssize_t      nbytes, bufsiz;
           struct stat  sb;

           if (argc != 2) {
               fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           if (lstat(argv[1], &sb) == -1) {
               perror("lstat");
               exit(EXIT_FAILURE);
           }

           /* Add one to the link size, so that we can determine whether
              the buffer returned by readlink() was truncated. */

           bufsiz = sb.st_size + 1;

           /* Some magic symlinks under (for example) /proc and /sys
              report 'st_size' as zero. In that case, take PATH_MAX as
              a "good enough" estimate. */

           if (sb.st_size == 0)
               bufsiz = PATH_MAX;

           buf = malloc(bufsiz);
           if (buf == NULL) {
               perror("malloc");
               exit(EXIT_FAILURE);
           }
           nbytes = readlink(argv[1], buf, bufsiz);
           if (nbytes == -1) {
               perror("readlink");
               exit(EXIT_FAILURE);
           }

           /* Print only 'nbytes' of 'buf', as it doesn't contain a terminating
              null byte ('\0'). */
           printf("'%s' points to '%.*s'\n", argv[1], (int) nbytes, buf);

           /* If the return value was equal to the buffer size, then
              the link target was larger than expected (perhaps because the
              target was changed between the call to lstat() and the call to
              readlink()). Warn the user that the returned target may have
              been truncated. */

           if (nbytes == bufsiz)
               printf("(Returned buffer may have been truncated)\n");

           free(buf);
           exit(EXIT_SUCCESS);
       }
