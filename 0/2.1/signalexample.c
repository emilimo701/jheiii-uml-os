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
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

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

//------------------------------------------------------------------------
int main(int argc, char *argv[])
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    processInput (argc, argv);
    return 0;
}


