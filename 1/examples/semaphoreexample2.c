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

#define DEFAULT_NTHREADS	2
#define DEFAULT_NIDS		1000000

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
#include <pthread.h>
#include <string.h> 


// struct to hold thread specific info; passed in to the thread callback
// function;
typedef struct SharedData_s {
    int threadId;
    int start;
    int end;
} SharedData;

void threadCallback ( void *ptr );

// struct to hold thread specific info; passed in to the thread callback
// function;
typedef struct ThreadData_s {
    int threadId;
    SharedData *shared_data;
} ThreadData;

// forward declarations
void handler (int sig);
void error (const char* msg);

// globals
int shmid;               /* shared memory ID */
SharedData *shared_data = NULL;
int sleepTime = DEFAULT_SLEEP_TIME;
int incr = DEFAULT_INCR;
bool bProducer = false;
bool bConsumer = false;
bool bThread = false;
bool bSemaphore = false;
int shmkey = DEFAULT_SHMKEY;
char semname[256];
sem_t *mutex;
ThreadData td;

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
void CreateSem (bool bCreate)
/* create semaphores                                                        */
//------------------------------------------------------------------------

{       /* CreateSem */
    int i;

    //create & initialize semaphore; use O_CREAT flag in only one process;
     if (bCreate) {
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

    if (shared_data) {
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
   char buf[1024];
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
    -t\n\
       use pthread\n\
\n\
    example#1: no semaphores [start these processes in two different windows]:\n\
      %s -p -s 5\n\
      %s -c -s 1 -i 5\n\
\n\
    example#2: with semaphores [start these processes in two different windows]:\n\
      %s -p -s 5 -x\n\
      %s -c -s 1 -i 5 -x\n\
\n\
    example#3: threads, no semaphores:\n\
      %s -t\n\
\n\
    example#4: threads, with semaphores:\n\
      %s -t -x\n\
\n";
    sprintf (buf, usageString, argv[0], DEFAULT_SLEEP_TIME, DEFAULT_SHMKEY, DEFAULT_SEMNAME, DEFAULT_INCR, 
        argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
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

    CreateSem (bProducer);
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
int startThreads (int nThreads, int nIds)
// main function to start the threads; sets up the arg for each thread and
// creates the thread; then waits for the threads to finish;
//------------------------------------------------------------------------
{
    CreateSem (true);

    // shared buffer--we keep it on the stack in this case but can be global or
    // on the heap too;
    SharedData sd;
    sd.start = 0;
    sd.end = 0;

    // allocate the thread and arg array
    pthread_t *t   = malloc (nThreads * sizeof (pthread_t));  
    if (t == NULL) {
        error ("could not malloc thread array");
    }

    ThreadData *td = malloc (nThreads * sizeof (ThreadData));
    if (t == NULL) {
        error ("could not malloc thread arg array");
    }
    int i;
    printf ("Starting %d threads with:\n", nThreads);
    printf ("\n\n");

    for (i = 0; i < nThreads; i++) {
        td[i].threadId = i;		// thread specific private data
        td[i].shared_data = &sd;	// KEY concept: we pass the same sd to all the threads;
    }
    printf ("\n\n");

    // create the threads 
    for (i = 0; i < nThreads; i++) {
        pthread_create ((&t[i]), NULL, (void *) &threadCallback, (void *) &(td[i]));
    }
    // wait for threads to end
    for (i = 0; i < nThreads; i++) {
        pthread_join(t[i], NULL);
    }
} 

//------------------------------------------------------------------------
void threadCallback ( void *ptr )
// thread callback function; do the real work for the thread in this function;
//------------------------------------------------------------------------
{
    ThreadData *td;
    td = (ThreadData*) ptr;
    // NOTE that td->shared_data is the shared buffer for all the threads;

    int id;
    printf("[Thread %d] start: %d end: %d\n", td->threadId, td->shared_data->start, td->shared_data->end);

    // print the id range;
    while (1) {
        // use semaphore if specified by command line arg;
        if (bSemaphore) sem_wait (mutex);

        // get the contents of shared memory into local variable x;
        int x = td->shared_data->start;

        // do some processing--simulate by sleep
        sleep (sleepTime);

        // now check that the value of shared memory is still the same--if not issue warning
        if (x != td->shared_data->start) {
            printf ("[Thread: %d] *******WARNING********x = %d td->shared_data->start=%d\n", td->threadId, x, td->shared_data->start);
        }

        // modify local copy and write it back to shared memory;
        x = x + incr;
        printf ("[Thread : %d] x = %d td->start=%d\n", td->threadId, x, td->shared_data->start);
        td->shared_data->start = x;

        // use semaphore if specified by command line arg;
        if (bSemaphore) sem_post (mutex);
       
        // this really needs to be a synchronizing semaphore; sleep will do for now 
        sleep (1);
    } 
    pthread_exit(0); /* exit */
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
 
    while ((c = getopt (argc, argv, "hs:i:pck:xm:t")) != -1) {
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
            case 't':
                bThread = true;
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
    if (bThread) {
        startThreads (DEFAULT_NTHREADS, DEFAULT_NIDS);
    }
    else {
        updateShared (0, 1000);
    }
    return 0;
}





