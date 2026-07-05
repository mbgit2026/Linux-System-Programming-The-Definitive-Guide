#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

int main(int argc, char *argv[])
{
	char buf[1024], *current_dir, hostname[1024];
	int ret, errno;
	uid_t userid; 
	gid_t groupid;
	pid_t pid;
	struct sysinfo info;

	if ((current_dir = getcwd (buf, 1024)) == NULL) {
		perror("getcwd");
		exit(1);
	}

	userid = getuid(); 
	groupid = getgid();

	ret = gethostname(hostname, 1024);
	if ( hostname == NULL) {
		perror("gethostname");
		exit (1);
	}

	
	pid = getpid();

	ret = sysinfo(&info);
	if ( ret == -1 ) {
		perror("sysinfo");
		exit (1);
	}

	printf ("\n");
	printf ("Current Work Directory: %s\n", buf);
	printf ("User ID: %d\n", userid);
	printf ("Group ID: %d\n", groupid);
	printf ("Hostname: %s\n", hostname);
	printf ("Process ID: %d\n", pid);
	printf ("Hours from the boot: %d\n", info.uptime / 3600);
	printf ("Total memory: %ld Kb\n", info.totalram / 1024);
	printf ("Free memory: %ld Kb\n", info.freeram / 1024);
	printf ("\n");

	return 0;
}


