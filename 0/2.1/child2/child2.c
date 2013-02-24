#include "problem2.1.h"

#define SHAKE_MSG	"hello"
#define PFC_UNUSED	-1


//------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------
{
   printf("[INFO]\tin child 2 (pid %d): c2 overlaid successfully.\n", getpid());
   signal(SIGTERM, sigterm_handler);

   char msgbuf[HW0BUFSIZ];

   mode_t nodetype_and_permissions = (S_IFIFO | READ_PERM); //set the node type to S_IFIFO with permissions (octal) 644

   // create named pipe
   int mknod_retval = mknod(FIFO_LOC, nodetype_and_permissions, (dev_t) MKNOD_UNUSED);

   // wait for parent to show up at other end of pipe
   //printf("[INFO]\tCHILD 2: Waitin on parent ... mknod_retval=%d\n", mknod_retval);
   int pipe_fd = open(FIFO_LOC, O_WRONLY);
   pipefailcheck(pipe_fd, "write-opening", getppid(), PFC_UNUSED);
   // parent is expecting us to initiate. write message to pipe
   printf("[EVENT]\tin child 2 (%d): about to send message to parent: \"%s\"\n", getpid(), SHAKE_MSG);
   pipefailcheck(write(pipe_fd, SHAKE_MSG, strlen(SHAKE_MSG)), "write to", getppid(), PFC_UNUSED);
   close(pipe_fd);

   //expecting a message back to complete handshake
   //sleep(12);
   //printf("IN child 2: waiting for parent to open pipe to write ack.\n");
   pipe_fd = open(FIFO_LOC, O_RDONLY);
   pipefailcheck(pipe_fd, "reeeeead-opening", getppid(), PFC_UNUSED);
   pipefailcheck(read(pipe_fd, msgbuf, HW0BUFSIZ), "read from", getppid(), PFC_UNUSED); close(pipe_fd);
   printf("[EVENT]\tin child 2 (pid %d): received message from parent (ppid=%d): \"%s\"\n", getpid(), getppid(), msgbuf);

   // done with pipe, so remove it
   pipefailcheck(remove(FIFO_LOC), "removal of", getppid(), PFC_UNUSED);

   printf("[EVENT]\tin child 2 (pid %d): Done; entering infinite loop.\n", getpid());
   while (1) {;}
}

