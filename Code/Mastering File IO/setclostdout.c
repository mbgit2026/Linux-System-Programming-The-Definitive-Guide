 #include <stdio.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <fcntl.h>

 int main (int argc, char *argv[])
 {
	if (argc < 2) {
		printf ("\nUsare %s 0|1", argv[0]);
		return 1;
    }
	
    int flags;
	int cloex = atoi(argv[1]);
	flags = fcntl (STDOUT_FILENO, F_GETFD);
	
    if (cloex == 0)
		flags = flags;
	else if (cloex == 1)
		flags |= FD_CLOEXEC;

	fcntl (STDOUT_FILENO, F_SETFD, flags);

	execlp ("ls", "ls", NULL);
	return 0;
  }