 #include <stdlib.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <stdio.h>

 #define FILEPERM (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH| S_IWOTH|S_IXOTH)
 #define MASKPERM (S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
 #define MAX (0777)

 int main(int argc, char *argv[]) 
 {
    struct stat sb;
    if (argc != 2) {
       printf("\nUsage: %s file_name", argv[0]);
 	   exit(EXIT_FAILURE);
    }	
    printf("\n---------------------------------------------------------\n"); 
    printf("\nFilePerm: \t\t%b", FILEPERM);
    printf("\nMaskPerm: \t\t%b", MASKPERM);
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    printf("\n\n[777 & MaskPerm]: %b & %b = %b",MAX,MASKPERM,MAX & MASKPERM);
    printf("\n[FilePerm - MaskPerm]: %b - %b = %b",FILEPERM,MASKPERM,FILEPERM - MASKPERM);

    if (creat(argv[1], FILEPERM) < 0)
       perror("creat");
    else {
      printf("\n\n-------------------------------------------------------\n");
 	  printf("\n%s created with permissions: %b",argv[1],FILEPERM - MASKPERM);
    }   
	
    lstat(argv[1], &sb);
    printf("\n\n%s perm from stat: %b", argv[1], sb.st_mode);

    return 0;
 }
