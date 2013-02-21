#include "problem2.1.h"

#define SHAKE_MSG	"hello"
#define PFC_UNUSED	-1
#define MKNOD_UNUSED	0
#define READ_PERM	0644


//------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------
{
   printf("in child 2 (pid %d): c2 overlaid successfully.\n", getpid());
   signal(SIGTERM, sigterm_handler);

   char msgbuf[HW0BUFSIZ];

   mode_t nodetype_and_permissions = (S_IFIFO | READ_PERM); //set the node type to S_IFIFO with permissions (octal) 644 to write

   // create named pipe
   pipefailcheck(mknod(FIFO_LOC, nodetype_and_permissions, (dev_t) MKNOD_UNUSED), "creation of", getppid(), PFC_UNUSED);

   // wait for parent to show up at other end of pipe
   printf("Waitin on parent\n");
   int pipe_fd = open(FIFO_LOC, O_WRONLY);
   pipefailcheck(pipe_fd, "write-opening", getppid(), PFC_UNUSED);
   // parent is expecting us to initiate. write message to pipe
   pipefailcheck(write(pipe_fd, SHAKE_MSG, strlen(SHAKE_MSG)), "write to", getppid(), PFC_UNUSED);
   close(pipe_fd);

   //expecting a message back to complete handshake
   //sleep(12);
   printf("IN child 2: waiting for parent to open pipe to write ack.\n");
   pipe_fd = open(FIFO_LOC, O_RDONLY);
   pipefailcheck(pipe_fd, "reeeeead-opening", getppid(), PFC_UNUSED);
   pipefailcheck(read(pipe_fd, msgbuf, HW0BUFSIZ), "read from", getppid(), PFC_UNUSED); close(pipe_fd);
   printf("in child 2 (pid %d): received message from parent (ppid=%d): \"%s\"\n", getpid(), getppid(), msgbuf);

   // done with pipe, so remove it
   pipefailcheck(remove(FIFO_LOC), "removal of", getppid(), PFC_UNUSED);

   printf("in child 2 (pid %d): entering infinite loop.\n", getpid());
   while (1) {;}
   











   

/*
   if (pid == 0) {
      // Code only executed by first child process
      signal(SIGTERM, sigterm_handler);
      g++; l++; (*p)++;
      char recv[64];
      printf("in first child (%d): about to write message to pipe: \"%s\"\n", getpid(), "hi");
      write(pipe_handle[1], "hi", 64);
      printf("in first child (%d): waiting 2 seconds for ack from parent\n", getpid());
      sleep(2); //wait for message back TODO remove sleep()
      read(pipe_handle[0], &recv, 64);
      printf("in first child (%d): received message through pipe: \"%s\"\n", getpid(), recv);
      printf("in first child (%d): ppid: %d\n", getpid (), getppid());
      printf("in first child (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, (*p));
      printf("in first child (%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);
      while (1) {;}
   }
   else if (pid < 0) {
      printf("in parent (%d): error in fork\n", getpid());
      exit(1);
   }
   else {
      // Code only executed by parent process
      //printf ("in parent(%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
      //printf ("in parent(%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);

      pid_t pid2 = fork();

      if (pid2 < 0) {
         printf("in parent(%d): error in fork\n", getpid());
         exit(1);
      }
      else if (pid2 > 0) {
         //code only executed by parent process

         char receive[64];
         sleep(1); //wait for message TODO remove sleep()
         printf("in parent (%d): waiting 1 second for message from child one\n", getpid());
         read(pipe_handle[0], receive, 64);
         if (*receive) {
            printf("in parent (%d): received: \"%s\"\n", getpid(), receive);
            printf("in parent (%d): about to write message to pipe: \"%s\"\n", getpid(), "copy");
            write(pipe_handle[1], "copy", 64);
         }
         else {
            printf("in parent (%d): nothing in buffer from child one.\n", getpid());
         }
         // TODO reverse
         //TODO handshake with child 2
         sleep(2); //wait for child to print its ack from us TODO remove this sleep()

         //printf ("in parent (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, *p);
         //printf ("in parent (%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);

         sleep(3); // "7. Parent will sleep for a few seconds..."
         printf("in parent (%d): sleeping for 3 seconds.\n", getpid());

         int status, status2;
         printf("in parent (%d): about to send signal %d to child with PID %d\n", getpid(), SIGTERM, pid);
         kill(pid, SIGTERM);
         waitpid(pid, &status, 0); //wait for SIGCHLD signal
         printf("in parent (%d): Child one terminated with exit status %d\n", getpid(), status);
         //printf("in parent (%d): about to send signal %d to child with PID %d\n", getpid(), SIGTERM, pid2);
         //kill(pid, SIGTERM); //TODO uncomment
         //waitpid(pid2, &status2, 0); //wait for SIGCHLD signal TODO options? , uncomment
         //printf("in parent (%d): Child two terminated with exit status %d\n\n\n", getpid(), status2); //TODO uncomment
         printf("in parent (%d): done\n\n", getpid());
      }
      else {
         //code only executed by second child process
         g--; l--; (*p)--; sleep(5);
         printf("in second child (%d): ppid: %d\n", getpid(), getppid());
         printf("in second child (%d): g = %d l = %d p = %d *p=%d\n", getpid(), g, l, p, (*p));
         printf("in second child (%d): Address: g = %ld l = %ld p = %ld\n", getpid(), &g, &l, &p);
         //TODO: exec
         //while (1) {;}
      }      
   }

   // Code executed by both parent and child.
*/
   exit(0);
}


