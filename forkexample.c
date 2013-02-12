//------------------------------------------------------------------------
// fork example code
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

int g = 0;    // global

//------------------------------------------------------------------------
main()
//------------------------------------------------------------------------
{
   int l = 10;    // local stack variable;

   int *p = malloc (sizeof (int));    // heap variable
   *p = 20;

   pid_t pid = fork();

   if (pid == 0) {               // child
 //     sleep (300);
      g++;
      l++;
      (*p)++;
      // Code only executed by child process
      printf ("in child: pid: %d\n", getpid ());
      printf ("g = %d l = %d p = %d *p=%d\n", g, l, p, *p);
      printf ("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);
    }
    else if (pid < 0) {            // failed to fork
        printf ("error in fork\n");
        exit(1);
    }
    else {                        // parent
//      sleep (300);
      // Code only executed by parent process

sleep (5);
      printf ("in parent: pid: %d\n", getpid ());
      printf ("g = %d l = %d p = %d *p=%d\n", g, l, p, *p);
      printf ("Address: g = %ld l = %ld p = %ld\n\n\n", &g, &l, &p);
    }

    // Code executed by both parent and child.
  
}
                

