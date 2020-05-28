#include "stdfx.h"

key_t key;

int id;
char *  memptr;
char  receiver[BUF_SIZE];
sem_t * mutex;
sem_t * sem_1;
sem_t * sem_2;

int main(int argc,char *argv[])
{
	key = KEY;
	if((id = shmget(key, BUF_SIZE, IPC_CREAT|0666))<0)
	{
		printf("shmget faail\n");
		exit(1);
	}
	if((mutex=sem_open(MUTEX_N,O_CREAT,0644,1))<0)
	{
		printf("sem_open fail\n");
		exit(1);
	}
	if((sem_1=sem_open(SEM_N1,O_CREAT,0644,0))<0)
	{
		printf("sem_open fail\n");
		exit(1);
	}
	if((sem_2=sem_open(SEM_N2,O_CREAT,0644,0))<0)
	{
		printf("sem_open fail\n");
		exit(1);
	}
	memptr=shmat(id,NULL,0);
	
	sem_wait(sem_1);
	sem_wait(mutex);
	strncpy(receiver,memptr,BUF_SIZE);
	sem_post(mutex);
	
	printf("RECEIVER:info from sender\n%s",receiver);
	
	strncpy(memptr,"over",4);
	sem_post(sem_2);
	printf("RECEIVER:-----------end----------\n");

	return 0;
}
