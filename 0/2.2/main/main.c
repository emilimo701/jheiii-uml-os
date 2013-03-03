#define _XOPEN_SOURCE	700
#define FILENAME_BUFSIZ	256

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

off_t disk_use(const char *path) {
   struct stat file_stat;
   stat(path, &file_stat);
   if (S_ISDIR(file_stat.st_mode)) {
      return (off_t) 0;
   } else {
      return file_stat.st_size;
   }
}

//------------------------------------------------------------------------
int main(int num_args, char *arguments[])
//------------------------------------------------------------------------
{
   if (num_args < 2) {
      printf("please specify path\n");
      exit(1);
   }
   
   char path[FILENAME_BUFSIZ];
   strcpy(path, arguments[1]); // TODO system macro?
   struct stat file_stat;
   
   stat(path, &file_stat);

   off_t num_bytes = file_stat.st_size;

   printf("%s: type %o, %d bytes or %d bytes\n", path, file_stat.st_mode, (int) num_bytes, (int) disk_use(path));
   
   exit(0);
}

