#ifndef CHILD_H
#define CHILD_H
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SHMKEY  859047     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )


#endif
