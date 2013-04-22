#include "main.h"

int usage (char *argv[])
{
   char buf[1024];
   char *usageString =
"%s: semaphore example program \n\
    -h\n\
       print this usage \n\
    -s <sleeptime> \n\
       how many seconds to sleep during each iteration; default is %d\n\
    -pp<n> <startId> <endId>\n\
       start producer process with arguments startId, endId where <n> is in [0-9a-zA-Z]\n\
    -pt<n> <startId> <endId>\n\
       start producer thread with arguments startId, endId where <n> is in [0-9a-zA-Z]\n\
    -cp <count>\n\
       start <count> consumer processes\n\
    -ct <count>\n\
       start <count> consumer threads\n\
\n";
    sprintf (buf, usageString, argv[0], DEFAULT_SLEEP_TIME);
    printf ("%s", buf);
    exit (0);
}

int str_to_int(const char *intptr) {
    int offset = 0;
    while ('\0' != *(intptr+offset)) {
        if (*(intptr+offset) < '0' || *(intptr+offset) > '9') jquit(1, "Error parsing positive, decimal integer.");
        offset++;
    }
    long int longval = strtol(intptr, (char **) NULL, 10);
    if (INT_MAX < longval) jquit(1, "Integer argument found greater than MAX_INT.");
    return (int) longval;
}

int registerProducer(const char *arg_n, const char *arg_np1, const char *arg_np2) {
    if (MAX_PRODUCERS <= pp_count) jquit(1, "Reached internal producer-arg limit of MAX_PRODUCERS.");
    char d;
    char string_01[64];
    strcpy(string_01, arg_n);
    d = *(string_01 + 2);
    switch (d) {
        case 'p':
            bProcess = true;
            break;
        case 't':
            bThread = true;
            break;
        default:
            return -1;
    }
    if ('\0' != *(string_01 + 4)) return -1;
    strcpy(pp[pp_count].name, arg_n);
    pp[pp_count].startID = str_to_int(arg_np1);
    pp[pp_count].endID = str_to_int(arg_np2);
    if (pp[pp_count].endID < pp[pp_count].startID) jquit(1, "Please enter nondecreasing ranges ony.");
    pp_count++;
    return 0;
}

int registerConsumers(const char *arg_n, const char *arg_np1) {
    char d;
    char string_01[64];
    strcpy(string_01, arg_n);
    d = *(string_01 + 2);
    switch (d) {
        case 'p':
            bProcess = true;
            break;
        case 't':
            bThread = true;
            break;
        default:
            return -1;
    }
    if ('\0' != *(string_01 + 3)) return -1;
    nConsumers = str_to_int(arg_np1);
    return 0;
}

int processInput (const int argc, char *argv[])
{
//  strcpy (semname, DEFAULT_SEMNAME);
    char string_01[64];
    char c;
    for (/*skip 1st argument (exe name)*/ int i = 1; i < argc; i++) {
        strcpy(string_01, argv[i]);
        if ('-' != *string_01)
            jquit(1, "Encountered error parsing arguments (expected hyphen).\nRun with -h for usage info.");
        c = *(string_01 + 1);
        switch (c) {
            case 'v':
                bVerbose = true;
                break;
            case 'h':
                bHelp = true;
                break;
            case 's':
                bStatus = true;
                break;
            case 'p':
                bool_producerArgFound = true;
                if (registerProducer(argv[i], argv[i+1], argv[i+2]) == -1) usage(argv);
                i += 2;
                break;
            case 'c':
                if (bool_consumerArgFound) usage(argv);
                bool_consumerArgFound = true;
                if (registerConsumers(argv[i], argv[i+1]) == -1) usage(argv);
                i += 1;
                break;
            default:
                //printf("Encountered unknown arguent: %s\n", argv[i]);
                usage(argv);
        }
    }

    // if we've made it this far, reprint the command and arguments
    if (bVerbose) {
        printf("%s", argv[0]);
        for (int i = 1; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n\n");
    }

    // verify that arguments follow rules
    if (nConsumers < 1) jquit(1, "Number of consumers must be greater than zero.");
    if (nConsumers > MAX_CONSUMERS) jquit(1, "Too many consumers.");
    if (!bool_producerArgFound || !bool_consumerArgFound)
        jquit(1, "Not enough arguments; must specify consumers and at least one producer.");
    if (bProcess == bThread) 
        jquit(1, "Must select either process or thread implementation, and not both.");
    for (int p = 0; *((pp+p)->name) != 0; p++) {
        if (bVerbose) printf("%s: [%d, %d]\n", (pp+p)->name, (pp+p)->startID, (pp+p)->endID);
        for (int q = p; q > 0; q--) {
            if (((pp+p)->startID >= (pp+q-1)->startID && (pp+p)->startID <= (pp+q-1)->endID)
                    || ((pp+p)->endID >= (pp+q-1)->startID && (pp+p)->endID <= (pp+q-1)->endID)
                    || ((pp+p)->startID < (pp+q-1)->startID && (pp+p)->endID > (pp+q-1)->endID)
                    )
                jquit(1, "Please enter non-overlapping producer ranges.");
            if (strcmp((pp+p)->name, (pp+q-1)->name) == 0)
                jquit(1, "Please enter unique producer IDs (-p[pt][0-9A-Za-z]\\0).");
        }
    }
    if (bVerbose) printf("%d Consumers.\n", nConsumers);
    
    return 0;
}

void helpStuff(char *argv[]) {
    printf("%s\n", "helpStuff()");
    usage(argv);
}

int main(int argc, char *argv[])
{
    //SignalStuff ();

    bool_producerArgFound = false;
    bool_consumerArgFound = false;
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
    bVerbose = false;
    shmkey = DEFAULT_SHMKEY;
    pp_count = 0;

    processInput (argc, argv);
    if (bHelp) {
        helpStuff(argv);
    }
    if (bStatus) {
        //TODO;
    }
    if (bVerbose) printf("\n\n\n\n");
    if (bProcess) {
        if (bVerbose) printf("Running Process implementation\n");
        startProducers();
        startConsumers();
        //updateShared (0, 1000);
    }
    if (bThread) {
        if (bVerbose) printf("Running Thread implementation\n");
        runThreadImpl();
        //startThreads (DEFAULT_NTHREADS, DEFAULT_NIDS);
    }
    return 0;//TODO: question... exit(0) or return 0 -- which is better to use (and in which circumstances)?
}

