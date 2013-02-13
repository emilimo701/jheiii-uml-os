//------------------------------------------------------------------------
// fork example code
// 94.308 Introduction to Operating Systems
// Spring 2012
// Department of Computer Science
// University of Massachusetts Lowell
//
// Instructor: Krishnan Seetharaman
//------------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h> 
#include <sys/wait.h>

int g = 0;    // global

//------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------
{
   int l = 10;    // local stack variable;

   int *p = malloc (sizeof (int));    // heap variable
   *p = 20;
   int pipe_handle[2];

   if (pipe(pipe_handle) < 0) {
      printf("error in pipe()\n");
      exit(1);
   }

   pid_t pid = fork();

   if (pid == 0) {               // child
      // Code only executed by first child process
      sleep(1);
      g++;
      l++;
      (*p)++;
      printf ("in first child: pid: %d, ppid: %d\n", getpid (), getppid());
      printf ("g = %d l = %d p = %d *p=%d\n", g, l, p, (*p));
      printf ("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);
      char *send = "hi";
      write(pipe_handle[1], send, 64);
    }
    else if (pid < 0) {            // failed to fork
        printf ("error in fork\n");
        exit(1);
    }
    else {                        // parent
      // Code only executed by parent process
      printf ("in parent: pid: %d\n", getpid ());
      printf ("g = %d l = %d p = %d *p=%d\n", g, l, p, *p);
      printf ("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);

      pid_t pid2 = fork();

      if (pid2 < 0) {
         printf("error in fork\n");
         exit(1);
      }
      else if (pid2 > 0) {
         //code only executed by parent process
         sleep(3);

         char receive[64];
         read(pipe_handle[0], receive, 64);
         if (*receive) {
            printf("in parent: received: %s\n\n\n", receive);
         }
         sleep(4);

         printf ("in parent: pid: %d\n", getpid ());
         printf ("g = %d l = %d p = %d *p=%d\n", g, l, p, *p);
         printf ("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);
         //int status;
         //wait(&status);
         //wait(&status);
      }
      else {
         //code only executed by second child process
         g--; l--; (*p)--; sleep(5);
         printf("in second child: pid: %d, ppid: %d\n", getpid(), getppid());
         printf("g = %d l = %d p = %d *p=%d\n", g, l, p, (*p));
         printf("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);
      }      
    }

    // Code executed by both parent and child.
    return 0;
}
                

