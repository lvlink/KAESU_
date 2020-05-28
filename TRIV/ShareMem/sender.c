#include "stdfx.h"

key_t key;

int id;
char* memptr;
char send[BUF_SIZE];

sem_t * mutex;
sem_t * sem_1;
sem_t * sem_2;

int main(int argc, char* argv[])
{
	key=KEY;
	if((id = shmget(key,BUF_SIZE,IPC_CREAT|0666))<0)
	{
		printf("shmget fail\n");
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
	
	printf("please type msg\n");
	read(0,send,BUF_SIZE);
	sem_wait(mutex);
	strncpy(memptr,send,BUF_SIZE);
	sem_post(mutex);
	sem_post(sem_1);
	
	printf("SENDER:meg send, waiting end meg\n");
	sem_wait(sem_2);
	strncpy(send,memptr,BUF_SIZE);
	printf("SENDER:info from RECEIVER:\n");
	printf("%s",memptr);
	printf("SENDER:---------end----------\n");
	
	msgctl(id,IPC_RMID,0);
	printf("del sharemem\n");
	sem_close(mutex);
	sem_unlink(MUTEX_N);
	sem_close(sem_1);
	sem_unlink(SEM_N1);
	sem_close(sem_2);
	sem_unlink(SEM_N2);
	return 0;
}
	
