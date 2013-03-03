#define _XOPEN_SOURCE	700
#define FILENAME_BUFSIZ	256 //TODO use predefined macro
#define PREFIX_DMSG "[DEBUG] "
//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

off_t disk_use(const char *path) {
   struct stat file_stat;
   if (stat(path, &file_stat)) {
      //perror(errno); // question: why doesn't errno get set to a failure?
      printf("Could not perform readdir(\"%s\").\n", path); 
      exit(EXIT_FAILURE);
   }
   if (S_ISDIR(file_stat.st_mode)) {
      struct dirent *dir_entry;
      DIR *dirptr;
      off_t sum = 0;
      dirptr = opendir(path);
      while (dir_entry = readdir(dirptr)) {
         char next_path[FILENAME_BUFSIZ];
         strcpy(next_path, path);
         strcat(next_path, dir_entry->d_name);
#ifdef DEBUG
         printf("%sEntry %s: type %d\n", PREFIX_DMSG, next_path, dir_entry->d_type);
#endif /* DEBUG */
         if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
            sum = sum + disk_use(next_path);
         }
      }
      closedir(dirptr);
#ifdef DEBUG
      printf("%s%s: %li\n", PREFIX_DMSG, path, (long) sum);
#endif /* DEBUG */
      return sum + file_stat.st_size;
   } else {
#ifdef DEBUG
      printf("%s%s: %li\n", PREFIX_DMSG, path, (long) file_stat.st_size);
#endif /* DEBUG */
      return file_stat.st_size;
   }
}


//------------------------------------------------------------------------
int main(int num_args, char *arguments[])
//------------------------------------------------------------------------
{
   if (num_args < 2) {
      printf("please specify path as second argument\n");
      exit(EXIT_FAILURE);
   }
   
   char path[FILENAME_BUFSIZ];
   strcpy(path, arguments[1]); // TODO system macro?
#ifdef DEBUG
   struct stat file_stat;
   
   stat(path, &file_stat);

   off_t num_bytes = file_stat.st_size;

   printf("%s%s: type %o, %d bytes or %li bytes\n\n", PREFIX_DMSG, path, file_stat.st_mode, (int) num_bytes, (long) disk_use(path));
#endif /* DEBUG */
   
#ifndef DEBUG
   printf("%li\t%s\n", (long) disk_use(path), path);
#endif /* ~DEBUG */

   exit(EXIT_SUCCESS);
}

