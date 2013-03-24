//------------------------------------------------------------------------
// pthread example code
// 94.308 Introduction to Operating Systems
// Spring 2012
// Department of Computer Science
// University of Massachusetts Lowell
//
// Instructor: Krishnan Seetharaman
//
// NOTE: To compile
//   gcc -g -o pth pthreadexample.c -lpthread
//
//------------------------------------------------------------------------

#define DEFAULT_NTHREADS	2
#define DEFAULT_NIDS		10

#include <unistd.h>     
#include <sys/types.h> 
#include <errno.h>    
#include <stdio.h>   
#include <stdlib.h> 
#include <pthread.h>
#include <string.h> 
#include <stdbool.h>

void threadCallback ( void *ptr );

// struct to hold thread specific info; passed in to the thread callback
// function;
typedef struct ThreadData_s {
    int threadId;
    int start;
    int end;
} ThreadData;

//------------------------------------------------------------------------
int usage (char *argv[])
//------------------------------------------------------------------------
{
   char buf[512];
   char *usageString =
"%s: pthread example program \n\
    -t: number of threads to start; default is %d\n\
    -n: how many ids per thread; default is %d\n\
    -h: print this usage \n\
\n";
    sprintf (buf, usageString, argv[0], DEFAULT_NTHREADS, DEFAULT_NIDS);
    printf (buf);
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
int startThreads (int nThreads, int nIds)
// main function to start the threads; sets up the arg for each thread and
// creates the thread; then waits for the threads to finish;
//------------------------------------------------------------------------
{
    // allocate the thread and arg array
    pthread_t *t   = malloc (nThreads * sizeof (pthread_t));  
    if (t == NULL) {
        error ("could not malloc thread array");
    }
    ThreadData *td = malloc (nThreads * sizeof (ThreadData));
    if (t == NULL) {
        error ("could not malloc thread arg array");
    }

    // set up the thread arguments; each thread gets an id and
    // start and end ids which are computed using the nIds parameter;
    // all threads get non-overlapping ranges; all threads get equal
    // range;
    int i;
    printf ("Starting %d threads with:\n", nThreads);
    for (i = 0; i < nThreads; i++) {
        int start = i*nIds;
        int end   = (i * nIds + nIds) - 1;
        printf("[Thread %d] start: %d end: %d\n", i, start, end);
        td[i].threadId = i;
        td[i].start = start;
        td[i].end   = end;
    } 
    printf ("\n\n");

    // create the threads 
    for (i = 0; i < nThreads; i++) {
        pthread_create (&(t[i]), NULL, (void *) &threadCallback, (void *) &(td[i]));
    }
    // wait for threads to end
    for (i = 0; i < nThreads; i++) {
        pthread_join(t[i], NULL);
    }
              
    exit(0);
} 

//------------------------------------------------------------------------
void threadCallback ( void *ptr )
// thread callback function; do the real work for the thread in this function;
//------------------------------------------------------------------------
{
    ThreadData *td;
    td = (ThreadData*) ptr;
    
    int id;
    printf("[Thread %d] start: %d end: %d\n", td->threadId, td->start, td->end);

    // print the id range;
    for (id = td->start; id <= td->end; id++) {
        printf("[Thread %d] id: %d\n", td->threadId, id);
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
    int nThreads = DEFAULT_NTHREADS;
    int nIds     = DEFAULT_NIDS;
 
    while ((c = getopt (argc, argv, "ht:n:")) != -1) {
        switch (c) {
            case 't':
                bT = true;
                nOptions++;
                nThreads = atoi (optarg); 	// need more error checking here;
                break;
            case 'h':
                bHelp = true;
                break;
            case 'n':
                bN = true;
                nOptions++;
                nIds = atoi (optarg);	// need more error checking here;
                break;
            default:
                abort ();
        }
    }

    // sanity check
    if (nOptions == 0) {
       printf ("NO option specified--using defaults\n\n");
    }
    if ((nThreads <= 0) || (nIds <= 0)) {
       printf ("Invalid value for -t or -n\n\n");
        usage (argv);
    }

    if (bHelp) {
        usage (argv);
    }
    startThreads (nThreads, nIds);
}

//------------------------------------------------------------------------
int main(int argc, char *argv[])
//------------------------------------------------------------------------
{
    printf ("pid: %d\n", getpid ());

    processInput (argc, argv);
    return 0;
}



