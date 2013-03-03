#include "hw0.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <signal.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define HW0BUFSIZ	64
#define FIFO_LOC	"/tmp/hw0handshake"
#define READ_PERM	0644
#define MKNOD_UNUSED	0

void sigterm_handler(int sigint); 
void sigusr2_handler(int sigint); 
void sigint_handler(int sigint);
void pipefailcheck(int retnval, char *operation, pid_t otherpid, pid_t anotherpid);

