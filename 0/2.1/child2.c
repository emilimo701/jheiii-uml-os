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
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h> 

#include <stdbool.h>
#include <signal.h>


#include <sys/stat.h>

#include <sys/wait.h>


//------------------------------------------------------------------------
int usage (char *argv[])
//------------------------------------------------------------------------
{
   char buf[512];
   char *usageString =
"%s: signal, fork example program \n\
    -a: set up all signals and wait for a signal to process; \n\
    -f: fork a child process and wait for SIGCHLD \n\
    -s: send a signal to another process \n\
    -h: print this usage \n\
\n";
    sprintf (buf, usageString, argv[0]);
    printf (buf);
    exit (0);
}

//------------------------------------------------------------------------
void sig_handler(int signum)
// generic signal handler; does nothing now except ack signal;
//------------------------------------------------------------------------
{
    printf ("[pid: %d] Received signal %d\n", getpid (), signum);
}

//------------------------------------------------------------------------
void sig_interrupt(int signum)
// specific handler for SIGINT; does nothing now except ack signal;
//------------------------------------------------------------------------
{
    printf ("[pid: %d] Received Ctrl-C%d\n", getpid (), signum);
}

//------------------------------------------------------------------------
int setupSignals ()
// setup signal handlers for all signals;
//------------------------------------------------------------------------
{
    int i;
    for (i = SIGHUP; i < SIGUNUSED; i++) {
        signal(i, sig_handler);
    }
    signal(SIGINT, sig_interrupt);
    return 0;
}

//------------------------------------------------------------------------
int sendSignal ()
// send signal to another process via kill system call;
//------------------------------------------------------------------------
{
    pid_t pid;
    int sig;

    printf ("enter pid of other process: ");
    scanf ("%d", &pid);
    printf ("enter signal to send: ");
    scanf ("%d", &sig);

    kill (pid, sig);
}

//------------------------------------------------------------------------
int doFork ()
// fork a child process and wait for the child to terminate and catch the
// signal;
//------------------------------------------------------------------------
{
    int status;
    pid_t w;
    pid_t pid = fork ();
    if (pid == 0) {    // child process;
        setupSignals ();
        printf ("[pid: %d] sleep in child for 300 sec\n", getpid ());
        sleep (300);
    }
    else if (pid < 0) {
        printf ("error in fork\n");
        exit(1);
    }
    else {    // parent process;
            printf ("[pid: %d] waitpid in parent for child (pid: %d) to exit\n", getpid (), pid);
            do {
                   w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
                   if (w == -1) {
                       char buf[256];
                       sprintf (buf, "[pid: %d] waitpid", getpid ());
                       perror (buf);
                       exit(EXIT_FAILURE);
                   }

                   if (WIFEXITED(status)) {
                       printf("[pid: %d] exited, status=%d\n", getpid (), WEXITSTATUS(status));
                   } else if (WIFSIGNALED(status)) {
                       printf("[pid: %d] killed by signal %d\n", getpid (), WTERMSIG(status));
                   } else if (WIFSTOPPED(status)) {
                       printf("[pid: %d] stopped by signal %d\n", getpid (), WSTOPSIG(status));
                   } else if (WIFCONTINUED(status)) {
                       printf("[pid: %d] continued\n");
                   }
               } while (!WIFEXITED(status) && !WIFSIGNALED(status));
               exit(EXIT_SUCCESS);
           }
}

//------------------------------------------------------------------------
int processInput (int argc, char *argv[])
// command line processing;
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    bool bHelp = false;
    bool bFork = false;
    bool bSignal = false;
    bool bAllSignals = false;
    char c;     
    int nOptions = 0;
 
    while ((c = getopt (argc, argv, "hsfa")) != -1) {
        switch (c) {
            case 'a':
                bAllSignals = true;
                nOptions++;
                break;
            case 'h':
                bHelp = true;
                break;
            case 'f':
                bFork = true;
                nOptions++;
                break;
            case 's':
                bSignal = true;
                nOptions++;
                break;
            default:
                abort ();
        }
    }

    // sanity check
    if (nOptions == 0) {
       printf ("NO option specified\n\n");
       usage (argv);
    }
    if (nOptions > 1) {
       printf ("Only one option can be specified\n\n");
       usage (argv);
    }

    if (bHelp) {
        usage (argv);
    }
    if (bAllSignals) {
        setupSignals ();
        sleep(300); // This is your chance to press CTRL-C
    } 
    if (bFork) {
        doFork ();
    }
    if (bSignal) {
        sendSignal ();
    }

}

/*
//------------------------------------------------------------------------
int main(int argc, char *argv[])
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    processInput (argc, argv);
    return 0;
}
*/

void sigterm_handler(int sigint) {
   printf("in process (%d): Received signal %d. Exiting.\n", getpid(), sigint);
   exit(0);
}

int g = 0;    // global

//------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------
{
   mknod("handshake", S_IFIFO | 0644 , 0);


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


