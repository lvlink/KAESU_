#include "stdfx.h"

int main(int argc, char* argv[])
{
	key_t key;
	int id;
	
	key=KEY;
	
	sem_open(MUTEX_N,O_CREAT,0644,1);
	sem_open(SEM_N1,O_CREAT,0644,0);
	sem_open(SEM_N2,O_CREAT,0644,0);
	
	id = shmget(key,BUF_SIZE,IPC_CREAT|0666);
	
	printf("Initialied\n");
	return 0;
}

