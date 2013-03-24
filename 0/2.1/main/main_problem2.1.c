//------------------------------------------------------------------------
// fork example code
// 94.308 Introduction to Operating Systems
// Spring 2012
// Department of Computer Science
// University of Massachusetts Lowell
//
// Instructor: Krishnan Seetharaman
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// signal, fork example code
// 94.308 Introduction to Operating Systems
// Spring 2012
// Department of Computer Science
// University of Massachusetts Lowell
//
// Instructor: Krishnan Seetharaman
//------------------------------------------------------------------------

#include "problem2.1.h"

int g = 0;    // global

//------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------
{
   signal(SIGTERM, sigterm_handler);
   signal(SIGUSR2, sigusr2_handler);
   signal(SIGINT, sigint_handler);

   printf("\nJohn Emilian homework 0, problem 2.1\n\n");

   int l = 10;    // local stack variable;

   int *p = malloc (sizeof (int));    // heap variable
   if (NULL == p) {
      fprintf(stderr, "Failed to allocate memory.\n");
      exit(EXIT_FAILURE);
   }
   *p = 20;
   int pipe_handle_msg[2];
   int pipe_handle_ack[2];

   if (pipe(pipe_handle_msg) == -1) {
      fprintf(stderr, "in parent (%d): error in pipe()\n", getpid());
      exit(EXIT_FAILURE);
   }

   if (pipe(pipe_handle_ack) == -1) {
      fprintf(stderr, "in parent (%d): error in pipe()\n", getpid());
      exit(EXIT_FAILURE);
   }

   pid_t pid = fork();

   if (pid == 0) {
      // Code only executed by first child process
      g++; l++; (*p)++;
      printf("[EVENT]\tin first child (%d): HELLO, WORLD! My ppid is %d\n", getpid (), getppid());
      char recv[HW0BUFSIZ];
      if (-1 == close(pipe_handle_msg[0])) { //returns a ssize_t but compiler doesn't require cast?
         fprintf(stderr, "C1: close(pipe_handle_msg[0] failed.\n");
         exit(EXIT_FAILURE);
      }
      if (close(pipe_handle_ack[1]) == -1) {
         fprintf(stderr, "C1: close(pipe_handle_ack[1] failed.\n");
         exit(EXIT_FAILURE);
      }
      printf("[EVENT]\tin first child (%d): writing message to pipe: \"%s\"\n", getpid(), "hi");
      if (-1 == write(pipe_handle_msg[1], "hi", HW0BUFSIZ)) {
         fprintf(stderr, "Error writing to named pipe in 1st child.\n");
         exit(EXIT_FAILURE);
      }
      if (-1 == read(pipe_handle_ack[0], &recv, HW0BUFSIZ)) {
         fprintf(stderr, "Error reading from named pipe in first child.\n");
         exit(EXIT_FAILURE);
      }

      if (close(pipe_handle_msg[1]) == -1) {
         fprintf(stderr, "C1: close(pipe_handle_msg[1] .\n");
         exit(EXIT_FAILURE);
      }

      if (close(pipe_handle_ack[0]) == -1) {
         fprintf(stderr, "C1: close(pipe_handle_ack[0] .\n");
         exit(EXIT_FAILURE);
      }
      printf("[EVENT]\tin first child (%d): received message through pipe: \"%s\"\n[EVENT]\tCHILD 1: Done; entering loop\n", getpid(), recv);
      while (1) {;} // question: will this burn CPU? starve other processes? or does compiler handle no-ops specially?
   }
   else if (pid < 0) {
      fprintf(stderr, "in parent (%d): error in fork\n", getpid());
      exit(EXIT_FAILURE);
   }
   else {
      // Code only executed by parent process
      //printf ("in parent(%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
      //printf ("in parent(%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);

      pid_t pid2 = fork();

      if (pid2 < 0) {
         fprintf(stderr, "in parent(%d): error in fork\n", getpid());
         if (kill(SIGTERM, pid) == -1) {
            fprintf(stderr, "%d failed sending sigterm to %d.\n", getpid(), pid);
         exit(EXIT_FAILURE);
      }
      else if (pid2 > 0) {
         //code only executed by parent process

         char receive[HW0BUFSIZ];


         // -----------------------------
         // Begin handshake with child 1.



         if (close(pipe_handle_msg[1]) == -1) {
            fprintf(stderr, "C2: close(pipe_handle_msg[1]  .\n");
            exit(EXIT_FAILURE);
         }

         if (close(pipe_handle_ack[0]) == -1) {
            fprintf(stderr, "C2: close(pipe_handle_ack[0] .\n");
            exit(EXIT_FAILURE);
         }

         if (-1 == read(pipe_handle_msg[0], &receive, HW0BUFSIZ)) {
            fprintf(stderr, "Error reading from named pipe in parent.\n");
            exit(EXIT_FAILURE);
         }

         // exclaim reception
         printf("[EVENT]\tin parent (%d): received message from child 1: \"%s\"\n", getpid(), receive);
         printf("[EVENT]\tin parent (%d): about to write message for child 1 to pipe: \"%s\"\n", getpid(), "copy");

         //write message back
         if (-1 == write(pipe_handle_ack[1], "copy", HW0BUFSIZ)) {
            fprintf(stderr, "Error writing to named pipe in parent.\n");
            exit(EXIT_FAILURE);
         }
         if (-1 == close(pipe_handle_msg[0])) {
            fprintf(stderr, "C2: close(pipe_handle_msg[0] failed.\n");
            exit(EXIT_FAILURE);
         }
         if (close(pipe_handle_ack[1]) == -1) {
            fprintf(stderr, "C2: close(pipe_handle_ack[1] failed.\n");
            exit(EXIT_FAILURE);
         }
         //erase buffer contents
         memset(receive, '\0', HW0BUFSIZ);
         //sleep(2); //wait for child 1 to print its ack from us TODO remove this sleep()?
         // End handshake with child 1.
         // ---------------------------


         // -----------------------------
         // Begin handshake with child 2.
         mode_t nodetype_and_permissions = (S_IFIFO | READ_PERM); //set the node type to S_IFIFO with permissions (octal) 644
         int mknod_retval = mknod(FIFO_LOC, nodetype_and_permissions, (dev_t) MKNOD_UNUSED);
         receive[0] = 'g'; receive[1] = 'o'; receive[2] = 't'; receive[3] = ':'; receive[4] = ' ';
         
         // expecting message from child 2. read it from named pipe
         //printf("[INFO]\tPARENT: Waitin child 2 ... mknod_retval=%d\n", mknod_retval);
         int pipe_fd = open(FIFO_LOC, O_RDONLY); //child should already have made it and be waiting by now
         pipefailcheck(pipe_fd, "read-opening", pid2, pid);
         pipefailcheck(read(pipe_fd, &receive[5], HW0BUFSIZ), "read from", pid2, pid);
         close(pipe_fd);
         printf("[EVENT]\tin parent (%d): received message from child 2: \"%s\"\n", getpid(), &receive[5]);

         // child 2 is expecting us to acknowledge
         pipe_fd = open(FIFO_LOC, O_WRONLY);
         pipefailcheck(pipe_fd, "write-opening", pid2, pid);
         // write ack to pipe
         printf("[EVENT]\tin parent (%d): about to send ack to child 2: \"%s\"\n", getpid(), receive);
         pipefailcheck(write(pipe_fd, receive, HW0BUFSIZ), "write to", pid2, pid);
         close(pipe_fd);
         // End handshake with child 2.
         // ---------------------------


         //printf ("in parent (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
         //printf ("in parent (%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);

         printf("[INFO]\tin parent (%d): sleeping for a few seconds.\n", getpid());
         sleep(3); // "7. Parent will sleep for a few seconds..."

         int status, status2;
         printf("[EVENT]\tin parent (%d): about to send signal %d to child with PID %d\n", getpid(), SIGTERM, pid);
         kill(pid, SIGTERM);
         waitpid(pid, &status, 0); //wait for SIGCHLD signal
         printf("[INFO]\tin parent (%d): Child one terminated with exit status %d\n", getpid(), status);
         printf("[EVENT]\tin parent (%d): about to send signal %d to child with PID %d\n", getpid(), SIGTERM, pid2);
         kill(pid2, SIGTERM);
         waitpid(pid2, &status2, 0); //wait for SIGCHLD signal
         printf("[INFO]\tin parent (%d): Child two terminated with exit status %d\n", getpid(), status2);
         printf("[EVENT]\tin parent (%d): done\n\n", getpid());
      }
      else {
         //code only executed by second child process
         printf("[EVENT]\tin second child (%d): HELLO, WORLD!!! My ppid is %d\n", getpid(), getppid());
         execlp("/home/john/uml/os/hw/0/2.1/child2/c2", "/home/john/uml/os/hw/0/2.1/child2/c2", NULL);
         printf("ERROR in child 2: execlp() returned -1\n");
         kill(getppid(), SIGTERM);
         kill(pid, SIGTERM);
         exit(EXIT_FAILURE);
      }      
   }

   // Code executed by both parent and child.
   exit(EXIT_SUCCESS);
}

