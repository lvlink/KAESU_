#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<semaphore.h>
#include<sys/types.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define BUF_SIZE 1024
#define KEY 1700

#define MUTEX_N "/home"
#define SEM_N1 "SEM_N1_"
#define SEM_N2 "SEM_N2_"
