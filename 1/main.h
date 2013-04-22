#define _XOPEN_SOURCE 500
#define DEFAULT_SLEEP_TIME	5
#define DEFAULT_INCR		1
#define DEFAULT_SHMKEY		1234
#define DEFAULT_SEMNAME		"foo"
#define DEFAULT_NTHREADS	2
#define DEFAULT_NIDS		1000000
#define DEFAULT_ERRCODE		28657
#define MAX_PRODUCERS		64
#define MAX_CONSUMERS		64

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
#include <limits.h>

// struct to hold thread specific info; passed in to the thread callback function
typedef struct SharedData_s {
    int threadId;
    int start;
    int end;
} SharedData;

void threadCallback ( void *ptr );

// struct to hold thread specific info; passed in to the thread callback function
typedef struct ThreadData_s {
    int threadId;
    SharedData *shared_data;
} ThreadData;

typedef struct {
    char name[8];
    int startID;
    int endID;
} Pending_Producer;

// forward declarations
void handler (int sig);
void error (const char* msg);
void jquit(int code, const char *message);
void CreateShMem();
void CreateSem(bool bCreate);
void SignalStuff();
int wrapsyscall(int retval, const char *errmsg);
void updateShared(int n1, int n2);
int startThreads(int nThreads, int nIds);
void threadCallback(void *ptr);
void startProducers();
void startConsumers();
void runThreadImpl();

// globals
int shmid;               /* shared memory ID */
SharedData *shared_data;
int sleepTime;
int incr;
bool bHelp;
bool bStatus;
bool bProducer;
bool bVerbose;
bool bConsumer;
bool bProcess;
bool bThread;
bool bSemaphore;
int shmkey;
char semname[256];
sem_t *mutex;
ThreadData td;
Pending_Producer pp[MAX_PRODUCERS];
int pp_count;
bool bool_producerArgFound;
bool bool_consumerArgFound;
int nConsumers;

