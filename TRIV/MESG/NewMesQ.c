#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/types.h>

#define BUF_SIZE 1024
#define L_BLUE               "\e[1;34m"
#define RED                  "\e[0;31m"
#define RESET                "\033[0m"
#define BLUE                 "\e[0;34m"
#define YELLOW               "\e[1;33m"
#define GREEN                "\e[0;32m"
#define L_PURPLE             "\e[1;35m"

struct msg{
	long mtype;
	char mtext[BUF_SIZE];
};
void* sender1(void *arg);
void* sender2(void *arg);
void* receiver(void *arg);

pthread_t id_t1,id_t2,id_t3;
pthread_mutex_t mutex;
sem_t sem1,sem2;
key_t key;
static int id,iscreate=0;
/*
normal info 6
shutdown info:
sender1:  1 <-> 2
sender2:  3 <-> 4
*/
int main(){
	pthread_mutex_init(&mutex,NULL);
	sem_init(&sem1,0,1);
	sem_init(&sem2,0,0);
	if(pthread_create(&id_t1,NULL,sender1,NULL)!=0){
		printf("sender1 fail\n");
		return 1;
	}
	if(pthread_create(&id_t2,NULL,sender2,NULL)!=0){
		printf("sender2 fail\n");
		return 1;
	}
	if(pthread_create(&id_t3,NULL,receiver,NULL)!=0){
		printf("receiver fail\n");
		return 1;
	}
	pthread_join(id_t1,NULL);
	pthread_join(id_t2,NULL);
	pthread_join(id_t3,NULL);
	pthread_mutex_destroy(&mutex);
	sem_destroy(&sem1);
	sem_destroy(&sem2);
	msgctl(id,IPC_RMID,NULL);
	return 0;
}

void* sender1(void *arg){

	key = ftok("/tmp",48);
	id = msgget(key,IPC_CREAT|0666);
	//printf(L_PURPLE"sender1 running!\n"RESET);
	struct msg info1;
	iscreate++;
	while(1){
        	sem_wait(&sem1);
		pthread_mutex_lock(&mutex);
		printf(YELLOW"SENDER1:type the msg\n"RESET);
		printf(RESET);
		fgets(info1.mtext,BUF_SIZE,stdin);
		info1.mtype=6;
		if(!strncmp(info1.mtext,"exit\n",5)){
			strcpy(info1.mtext,"end1");
			break;
		}
		msgsnd(id,(void*)&info1,BUF_SIZE,0);
		pthread_mutex_unlock(&mutex);
		printf(YELLOW"SNEDER1:send msg\n"RESET);
        	sem_post(&sem2);
	}
	info1.mtype=1;
	msgsnd(id,(void*)&info1,BUF_SIZE,0);
	printf(RED"SNEDER1:end msg send\n"RESET);
	pthread_mutex_unlock(&mutex);
	sem_post(&sem2);
	msgrcv(id,(void*)&info1,BUF_SIZE,2,0);
	if(!strncmp(info1.mtext,"over1",5)){
		printf(RED"SENDER1:Msg from RECEIVER\n");
		write(1,info1.mtext,5);
		printf("\n");
		printf("SENDER1:finished\n"RESET);
	}
	else{
		printf(RED"SENDER1:Wrong msg\n");
		write(1,info1.mtext,5);
		printf("\n"RESET);
	}
	return NULL;
}

void* sender2(void *arg){
	while(!iscreate);
	//printf(L_PURPLE"sender2 running\n"RESET);
	struct msg info2;
	while(1){
        	sem_wait(&sem1);
		pthread_mutex_lock(&mutex);
		printf(GREEN"SENDER2:type the msg\n"RESET);
		fgets(info2.mtext,BUF_SIZE,stdin);
		info2.mtype=6;
		if(!strncmp(info2.mtext,"exit\n",5)){
			strcpy(info2.mtext,"end2");
			break;
		}
		msgsnd(id,(void*)&info2,BUF_SIZE,0);
		pthread_mutex_unlock(&mutex);
		printf(GREEN"SENDER2:send msg\n"RESET);
		sem_post(&sem2);
	}
	info2.mtype=3;
	msgsnd(id,(void*)&info2,BUF_SIZE,0);
	printf(RED"SENDER2:end msg send\n"RESET);
	pthread_mutex_unlock(&mutex);
	sem_post(&sem2);
	msgrcv(id,(void*)&info2,BUF_SIZE,4,0);
	if(!strncmp(info2.mtext,"over2",5)){
		printf(RED"SENDER2:Msg from RECEIVER\n");
		write(1,info2.mtext,5);
		printf("\n");
		printf("SENDER2:finished\n"RESET);
	}
	else{
		printf(RED"SENDER2:Wrong msg\n");
		write(1,info2.mtext,5);
		printf("\n"RESET);
	}
	return NULL;
}


void* receiver(void *arg){
	while(!iscreate);
	//printf(L_PURPLE"receiver running\n"RESET);
	struct msg infor;
	int count = 2;
	while(count){
        	sem_wait(&sem2);
		pthread_mutex_lock(&mutex);
		msgrcv(id,(void*)&infor,BUF_SIZE,0,0);
		if(infor.mtype==1){
			if(!strncmp(infor.mtext,"end1",4)){
				printf(RED"RECEIVER:shutdown info from sender1\n"RESET);
				infor.mtype=2;
				strcpy(infor.mtext,"over1");
				msgsnd(id,(void*)&infor,BUF_SIZE,0);
			}
			else{
				printf(RED"RECEIVER:Wrong msg\n");
				write(1,infor.mtext,5);
				printf(RESET);
			}
			count--;
			pthread_mutex_unlock(&mutex);
			sem_post(&sem1);
			continue;
			}
		if(infor.mtype==3){
			if(!strncmp(infor.mtext,"end2",4)){
				printf(RED"RECEIVER:shutdown info from sender2\n"RESET);
				infor.mtype=4;
				strcpy(infor.mtext,"over2");
				msgsnd(id,(void*)&infor,BUF_SIZE,0);
			}
			else{
				printf(RED"RECEIVER:Wrong msg\n");
				write(1,infor.mtext,5);
				printf(RESET);
			}
			count--;
			pthread_mutex_unlock(&mutex);
	       	 	sem_post(&sem1);
			continue;
			}
		printf(BLUE"RECEIVER:info from sender\n"RESET);
		//write(1,infor.mtext,BUF_SIZE);
		printf(L_BLUE"%s" RESET,infor.mtext);
		pthread_mutex_unlock(&mutex);
	        sem_post(&sem1);
		}
	printf(RED"RECEIVER:end...\n"RESET);
	return NULL;
}
