#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include  <sys/ipc.h>

void terminateSigHandler(int);
void timeoutSigHandler(int); 

#define SHMKEY  859047     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )

int* mem_ptr;
extern int errno;

