#include "main.h"

int usage (char *argv[])
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

int registerProducer(const char *arg_n, const char *arg_np1, const char *arg_np2) {
    char d;
    char string_01[64];
    strcpy(string_01, arg_n);
                d = *(string_01 + 2);
                switch (d) {
                    case 'p':
                         if (bThread) {
                             printf("%s\n", "Please supply process xor thread arguments.");
                             return -1;
                         }
                         {
                         char ppp_name[strlen(arg_n)];
                         strcpy(ppp_name, arg_n);
                         Pending_Producer_Process ppp_struct = {.name = ppp_name, .startID = atoi(arg_np1), .endID = atoi(arg_np2)};
                         }
                         bProcess = true;
                         break;
                    case 't':
                         if (bProcess) {
                             printf("%s\n", "process xor thread");
                             return -1;
                         }
                         bThread = true;
                         //TODO
                         break;
                    default:
                         printf("%s\n", "what?");
                         return -1;
                }
    return 0;
}

int registerConsumers(const char *arg_n, const char *arg_np1) {
    char d;
    char string_01[64];
    strcpy(string_01, arg_n);
                d = *(string_01 + 2);
                switch (d) {
                    case 'p':
                         if (bThread) {
                             printf("%s\n", "Please supply process xor thread arguments.");
                             return -1;
                         }
                         bProcess = true;
                         //TODO
                         break;
                    case 't':
                         if (bProcess) {
                             printf("%s\n", "process xor thread");
                             return -1;
                         }
                         bThread = true;
                         //TODO
                         break;
                    default:
                         printf("%s\n", "what?");
                         return -1;
                }
    return 0;
}

void handleUnknownArg() {printf("%s\n", "handleUnknownArg()");}

int processInput (const int argc, char *argv[])
// command line processing;
{
//  strcpy (semname, DEFAULT_SEMNAME);
    char string_01[64];
    char c;

    for (/*skip 1st argument (exe name)*/ int i = 1; i < argc; i++) {
        strcpy(string_01, argv[i]);
        if ('-' != *string_01) {
            jquit(1, "Encountered error parsing arguments (expected hyphen).\nRun with -h for usage info.");
        }
        c = *(string_01 + 1);
        switch (c) {
            case 'h':
                bHelp = true;
                printf("%s\n", "TODO: print help and exit.");
                break;
            case 's':
                bStatus = true;
                printf("%s\n", "TODO: print status.");
                break;
            case 'p':
                registerProducer(argv[i], argv[i+1], argv[i+2]);
                i += 2;
                break;
            case 'c':
                registerConsumers(argv[i], argv[i+1]);
                i += 1;
                break;
            default:
                handleUnknownArg();
                jquit(1, "please try again.");
        }
    }

    if (bProcess == bThread) {
        jquit(1, "Must select process XOR thread implementation.");
    }
    return 0;
}

void helpStuff() {printf("%s\n", "helpStuff()");}

int main(int argc, char *argv[])
{
    shared_data = NULL;
    sleepTime = DEFAULT_SLEEP_TIME;
    incr = DEFAULT_INCR;
    bHelp = false;
    bStatus = false;
    bProducer = false;
    bConsumer = false;
    bProcess = false;
    bThread = false;
    bSemaphore = false;
    shmkey = DEFAULT_SHMKEY;
    ppp_count = 0;

    processInput (argc, argv);
    if (bHelp) {
        helpStuff();
    }
    if (bProcess && !bThread) {
        startProducers();
        startConsumers();
    }
    else {
        runThreadImpl();
    }
    //SignalStuff ();
    if (bThread) {
        printf("Running Thread implementation\n");
        //startThreads (DEFAULT_NTHREADS, DEFAULT_NIDS);
    }
    else {
        printf("Running Process implementation\n");
        //updateShared (0, 1000);
    }
    return 0;//TODO: question... exit(0) or return 0 -- which is better to use (and in which circumstances)?
}

