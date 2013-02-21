#include "problem2.1.h"

void sigterm_handler(int sigint) {
   printf("in process (%d): Received signal %d (SIGTERM). Exiting cleanly.\n", getpid(), sigint);
   exit(EXIT_SUCCESS);
}

void sigusr2_handler(int sigint) {
   printf("in process (%d): Received signal %d from other failed process, exiting.\n", getpid(), sigint);
   exit(EXIT_FAILURE);
}

void sigint_handler(int sigint) {
   printf("Process %d terminated by user.\n", getpid());
   remove(FIFO_LOC);
   exit(EXIT_FAILURE);
}

void pipefailcheck(int retnval, char *operation, pid_t otherpid, pid_t anotherpid) {
   if (retnval == -1) {
      printf("ERROR in process %d: %s named pipe %s failed.\n", getpid(), operation, FIFO_LOC);
      kill(otherpid, SIGUSR2);
      if (anotherpid != -1) {
         kill(anotherpid, SIGUSR2);
      }
      remove(FIFO_LOC);
      exit(EXIT_FAILURE);
   }
}

