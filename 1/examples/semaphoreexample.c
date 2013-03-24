//------------------------------------------------------------------------
// semaphore example code
// 94.308 Introduction to Operating Systems
// Spring 2012
// Department of Computer Science
// University of Massachusetts Lowell
//
// Instructor: Krishnan Seetharaman
//
// NOTE: To compile
//   gcc -g -o sem semaphoreexample.c -lpthread
//
//------------------------------------------------------------------------

/* -------------------------------------------------------------------------*/
/* constants								    */
/* -------------------------------------------------------------------------*/

#define DEFAULT_SLEEP_TIME	5
#define DEFAULT_INCR		1
#define DEFAULT_SHMKEY		1234
#define DEFAULT_SEMNAME		"foo"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>


// struct to hold thread specific info; passed in to the thread callback
// function;
typedef struct SharedData_s {
    int threadId;
    int start;
    int end;
} SharedData;

// forward declarations
void handler (int sig);
void error (const char* msg);

// globals
int shmid;               /* shared memory ID */
SharedData *shared_data;
int sleepTime = DEFAULT_SLEEP_TIME;
int incr = DEFAULT_INCR;
bool bProducer = false;
bool bConsumer = false;
bool bSemaphore = false;
int shmkey = DEFAULT_SHMKEY;
char semname[256];
sem_t *mutex;

//------------------------------------------------------------------------
void CreateShMem (void)
/* create and attach shared memory                                          */
//------------------------------------------------------------------------
{       /* CreateShMem */
    /* get shared memory */
    int shmflag = 0666;

    // use create flag in only one proecess;
    if (bProducer) {
         shmflag = shmflag | IPC_CREAT;
    }
//printf ("shmflag=%d\n", shmflag);
    if ((shmid = shmget (shmkey, sizeof (SharedData), shmflag)) == -1) {
        error ("shared get failed ..");
    }
    else {
        printf ("Shared Memory Id (key: %d): %d created ...\n", shmkey, shmid);
    }

    /* attach it to shared_data */
    if ((shared_data = (SharedData*) shmat (shmid, (char*) NULL, 0)) == (SharedData*) -1) {
        error ("shared get failed ..");
        handler (-1);
    }
    printf ("Shared Memory Id: %d attached ...\n", shmid);

    // clear the shared memory contents--only in one process;
    if (bProducer) {
        if (bSemaphore) sem_wait (mutex);
        memset(shared_data,0,sizeof (SharedData));
        if (bSemaphore) sem_post (mutex);
        printf ("Shared Memory Id: %d cleared ...\n", shmid);
    }
    printf ("\n");
}       /* CreateShMem */

//------------------------------------------------------------------------
void CreateSem (void)
/* create semaphores                                                        */
//------------------------------------------------------------------------

{       /* CreateSem */
    int i;

    //create & initialize semaphore; use O_CREAT flag in only one process;
     if (bProducer) {
         mutex = sem_open(semname,O_CREAT,0644,1);
     }
     else {
         mutex = sem_open(semname,0,0644,0);
     }
     if (mutex == SEM_FAILED) {
         perror("unable to create semaphore");
         sem_unlink(semname);
         exit(-1);
    }
    printf ("Semaphore \"%s\" created\n", semname);
}       /* CreateSem */

//------------------------------------------------------------------------
void SignalStuff ()
/* do signal stuff - select signal types, and set up handler for them       */
//------------------------------------------------------------------------

{       /* SignalStuff */
    sigset_t mask_sigs;
    int i, nsigs;
    /* signal types we are interested in */
    int sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTERM, SIGBUS,
                  SIGSEGV, SIGFPE};
    struct sigaction new;
   
    nsigs = sizeof (sigs) / sizeof (int);
    sigemptyset (&mask_sigs);
   
    /* set up signal types */
    for (i = 0; i < nsigs; i++) {
        sigaddset (&mask_sigs, sigs[i]);
    }
 
    /* set up handler for each signal type */
    for (i = 0; i < nsigs; i++) {
        new.sa_handler = handler;
        new.sa_mask    = mask_sigs;
        new.sa_flags   = SA_RESTART;
        if (sigaction (sigs[i], &new, NULL) == -1) {
            error ("Cant set signals ...");
            exit (1);
        }
    }
}       /* SignalStuff */

//------------------------------------------------------------------------
void handler (int sig)
/* signal handler for all signals. performs graceful termination            */
//------------------------------------------------------------------------

{       /* handler */
    int i, j, k;

    printf ("In signal handler with signal # %d\n", sig);

    /* Detach the shared memory segment */
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(1);
    }
    /* remove shared memory */
    if (shmctl (shmid, IPC_RMID, 0) == -1) {
        error ("Handler failed ...");
    }
    else {
        printf ("Shared Memory Id: %d removed ...\n", shmid);
    }

    // clean up the semaphore;
    sem_close (mutex);
    if (bProducer) {
       sem_unlink (semname);
       printf ("semaphore Id: %s removed ...\n", semname);
    }

    exit (5);
}       /* handler */

//------------------------------------------------------------------------
int usage (char *argv[])
//------------------------------------------------------------------------
{
   char buf[512];
   char *usageString =
"%s: semaphore example program \n\
    -h: print this usage \n\
    -s <sleeptime> \n\
       how many seconds to sleep during each iteration; default is %d\n\
    -k <shmkey>\n\
       shared memory key to use; default is %d\n\
    -m <semname>\n\
       semaphore name to use; default is \"%s\"\n\
    -i <incr>\n\
       increment to use during each iteration; default is %d\n\
    -p\n\
       this process is producer\n\
    -c\n\
       this process is consumer\n\
    -x\n\
       use semaphores\n\
\n\
    example#1: no semaphores [start these processes in two different windows]:\n\
      %s -p -s 5\n\
      %s -c -s 1 -i 5\n\
\n\
    example#2: with semaphores [start these processes in two different windows]:\n\
      %s -p -s 5 -x\n\
      %s -c -s 1 -i 5 -x\n\
\n";
    sprintf (buf, usageString, argv[0], DEFAULT_SLEEP_TIME, DEFAULT_SHMKEY, DEFAULT_SEMNAME, DEFAULT_INCR, argv[0], argv[0], argv[0], argv[0]);
    printf ("%s", buf);
    exit (0);
}

//------------------------------------------------------------------------
void error (const char* msg)
// display error message and exit with non-zero error code;
//------------------------------------------------------------------------
{
    printf ("ERROR: %s\n", msg);
    exit (1);
}

//------------------------------------------------------------------------
void updateShared (int n1, int n2)
// main routine; both processes write to and read from shared memory;
//------------------------------------------------------------------------
{
    int x;

    CreateSem ();
    CreateShMem ();

    while (1) {
        // use semaphore if specified by command line arg;
        if (bSemaphore) sem_wait (mutex);

        // get the contents of shared memory into local variable x;
        int x = shared_data->start;

        // do some processing--simulate by sleep
        sleep (sleepTime);

        // now check that the value of shared memory is still the same--if not issue warning
        if (x != shared_data->start) {
            printf ("[pid: %d] *******WARNING********x = %d shared_data->start=%d\n", getpid (), x, shared_data->start);
        }

        // modify local copy and write it back to shared memory;
        x = x + incr;
        printf ("[pid: %d] x = %d shared_data->start=%d\n", getpid (), x, shared_data->start);
        shared_data->start = x;

        // use semaphore if specified by command line arg;
        if (bSemaphore) sem_post (mutex);
       
        // this really needs to be a synchronizing semaphore; sleep will do for now 
        sleep (1);
    }
}

//------------------------------------------------------------------------
int processInput (int argc, char *argv[])
// command line processing;
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    bool bHelp = false;
    bool bT = false;
    bool bN = false;
    char c;     
    int nOptions = 0;

    strcpy (semname, DEFAULT_SEMNAME);
 
    while ((c = getopt (argc, argv, "hs:i:pck:xm:")) != -1) {
        switch (c) {
            case 'h':
                bHelp = true;
                break;
            case 's':
                sleepTime = atoi (optarg);
                break;
            case 'm':
                strcpy (semname,optarg);
                break;
            case 'k':
                shmkey = atoi (optarg);
                break;
            case 'i':
                incr = atoi (optarg);
                break;
            case 'p':
                bProducer = true;
                break;
            case 'c':
                bConsumer = true;
                break;
            case 'x':
                bSemaphore = true;
                break;
            default:
                abort ();
        }
    }

    // sanity check
    if (nOptions == 0) {
       printf ("NO option specified--using defaults\n\n");
    }

    if (bHelp) {
        usage (argv);
    }
}

//------------------------------------------------------------------------
int main(int argc, char *argv[])
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    processInput (argc, argv);
    SignalStuff ();
    updateShared (0, 1000);

    return 0;
}



