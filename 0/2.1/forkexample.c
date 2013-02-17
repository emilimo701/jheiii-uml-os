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
#include <signal.h>

void handler(int sigint) {
   printf("in process (%d): Received signal %d. Exiting.\n\n\n", getpid(), sigint);
   exit(0);
}

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
      printf("in parent (%d): error in pipe()\n", getpid());
      exit(1);
   }

   pid_t pid = fork();

   if (pid == 0) {
      // Code only executed by first child process
      signal(SIGTERM, handler);
      sleep(1);
      g++;
      l++;
      (*p)++;
      printf ("in first child (%d): ppid: %d\n", getpid (), getppid());
      printf ("in first child (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, (*p));
      printf ("in first child (%d): Address: g = %ld l = %ld p = %ld\n\n\n", getpid(), &g, &l, &p);
      char recv[64];
      write(pipe_handle[1], "hi", 64);
      sleep(4);
      read(pipe_handle[0], &recv, 64);
      printf("in first child (%d): received: %s\n\n", getpid(), recv);
      while (1) {;}
   }
   else if (pid < 0) {
      printf ("in parent (%d): error in fork\n", getpid());
      exit(1);
   }
   else {
      // Code only executed by parent process
      printf ("in parent(%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
      printf ("in parent(%d): Address: g = %ld l = %ld p = %ld\n\n\n", getpid(), &g, &l, &p);

      pid_t pid2 = fork();

      if (pid2 < 0) {
         printf("in parent(%d): error in fork\n", getpid());
         exit(1);
      }
      else if (pid2 > 0) {
         //code only executed by parent process
         sleep(3);

         char receive[64];
         read(pipe_handle[0], receive, 64);
         if (*receive) {
            printf("in parent (%d): received: %s\n\n\n", getpid(), receive);
            write(pipe_handle[1], "ack", 64);
         }
         //TODO handshake with child 2
         sleep(4);

         printf ("in parent (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
         printf ("in parent (%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);
         int status;
         //wait(&status);
         //wait(&status);
         kill(pid, SIGTERM); //TODO also send to child 2
         waitpid(pid, &status, 0); //wait for SIGCHLD signal TODO also wait on child 2
         printf("in parent (%d): Child one terminated with exit status %d\n\n\n", getpid(), status);
      }
      else {
         //code only executed by second child process
         g--; l--; (*p)--; sleep(5);
         printf("in second child (%d): ppid: %d\n", getpid(), getppid());
         printf("in second child (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, (*p));
         printf("in second child (%d): Address: g = %ld l = %ld p = %ld\n\n\n", getpid(), &g, &l, &p);
         //TODO: exec
         //while (1) {;}
      }      
   }

   // Code executed by both parent and child.
   exit(0);
}


