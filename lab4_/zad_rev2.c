#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int i;
int acount=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condVar  = PTHREAD_COND_INITIALIZER;

int accessible = 1;


void waitMySemaphor(int amountToWithdraw);
void signalMySemphor();
void *threadClientWithdrawer(void* args);
void *threadClientDeposit(void* args);
void *threadAccount();

void *threadClientWithdrawer(void* args)
{
	char* name = (char*) args;
	int amountToWithdraw = atoi(name);
	int i;
	for (i = 0; i< 10; ++i)
	{
		waitMySemaphor(amountToWithdraw);		
		if (acount >= amountToWithdraw)
		{
			printf("%d - %d\n", acount, amountToWithdraw);
			acount -= amountToWithdraw;
		}
		else
			printf("%s : To little in account\n",name);
		int sleepTime = rand()%500;
		usleep((sleepTime + 1250) * 1000);
		signalMySemphor();
		sleepTime = rand()%500;
		usleep((sleepTime + 1000) * 1000);
		
	}
	printf("%s : end of thread  -- acount: %d\n",name,acount);
}
void *threadClientDeposit(void* args)
{
	char* name = (char*) args;
	int amountToDeposit = atoi(name);
	int i;
	for (i = 0; i< 110; ++i)
	{
		int sleepTime = rand()%500;
		usleep((sleepTime + 250) * 1000);
		acount += amountToDeposit;
		signalMySemphor();		
	}
	printf("%s : end of thread  -- acount: %d\n",name,acount);
}
void *threadAccount()
{
	int i;
	int oldAcount;
	while(1)
	{
		if (oldAcount != acount)
		{
			printf("Acount amount: %d\n",acount);
			oldAcount = acount;
		}
	}
}

void waitMySemaphor(int amountToWithdraw)
{
	pthread_mutex_lock(&mutex);
	while  (accessible == 0 || acount < amountToWithdraw)
		pthread_cond_wait(&condVar, &mutex);
	accessible = 0;
	pthread_mutex_unlock(&mutex);
}

void signalMySemphor()
{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&condVar);
	accessible = 1;
	pthread_mutex_unlock(&mutex);
}



int main(int argc, char* argv[])
{
	pthread_t a,b,c,d,e,f,g,h,j;
	char* buff1 = (char*) malloc(15);
	char* buff2 = (char*) malloc(15);
	char* buff3 = (char*) malloc(15);
	char* buff4 = (char*) malloc(15);
	char* buff5 = (char*) malloc(15);
	char* buff6 = (char*) malloc(15);
	char* buff7 = (char*) malloc(15);
	char* buff8 = (char*) malloc(15);
	sprintf(buff1,"10");
	sprintf(buff2,"20");
	sprintf(buff3,"30");
	sprintf(buff4,"40");
	sprintf(buff5,"110");
	sprintf(buff6,"220");
	sprintf(buff7,"330");
	sprintf(buff8,"440");
	
	srand(time(NULL));
	
	
	
	pthread_create(&a,NULL,threadAccount,NULL);
	pthread_create(&b,NULL,threadClientDeposit,buff1);
	pthread_create(&c,NULL,threadClientDeposit,buff2);
	pthread_create(&d,NULL,threadClientDeposit,buff3);
	pthread_create(&e,NULL,threadClientDeposit,buff4);
	pthread_create(&f,NULL,threadClientWithdrawer,buff5);
	pthread_create(&g,NULL,threadClientWithdrawer,buff6);
	pthread_create(&h,NULL,threadClientWithdrawer,buff7);
	pthread_create(&j,NULL,threadClientWithdrawer,buff8);
	
	pthread_join(a,NULL);
	pthread_join(b,NULL);
	pthread_join(c,NULL);
	pthread_join(d,NULL);
	pthread_join(e,NULL);
	pthread_join(f,NULL);
	pthread_join(g,NULL);
	pthread_join(h,NULL);
	pthread_join(j,NULL);
	
	
	
	
}
