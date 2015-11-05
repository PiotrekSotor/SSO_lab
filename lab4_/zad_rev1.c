#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

int i;
int acount=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condVar  = PTHREAD_COND_INITIALIZER;

int accessible = 1;


void waitMySemaphor();
void signalMySemphor();
void *threadClient(void* args);
void *threadAccount();

void *threadClient(void* args)
{
	char* name = (char*) args;
	int i;
	for (i = 0; i< 10; ++i)
	{
		
		//printf("%s: before critical section\n",name);
		waitMySemaphor();
		
		
		printf("%s: before op: %d\n",name,acount);
		if (acount == 0)
		{
			sleep(1);
			++acount;
		}
		else
		{
			sleep(1);
			--acount;
		}
		printf("%s: after op: %d\n",name,acount);
		
			
		signalMySemphor();
		//printf("%s: after critical section\n",name);
		sleep(1);
		
	}
}
void *threadAccount()
{
	int i;
	for (i = 0; i< 10; ++i)
	{
		//printf("threadAccount %d\n",i);
		sleep(1);
	}
}

void waitMySemaphor()
{
	pthread_mutex_lock(&mutex);
	while  (accessible == 0)
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
	pthread_t a,b,c,d,e;
	char* buff1 = (char*) malloc(15);
	char* buff2 = (char*) malloc(15);
	char* buff3 = (char*) malloc(15);
	char* buff4 = (char*) malloc(15);
	sprintf(buff1,"thread_1");
	sprintf(buff2,"thread_2");
	sprintf(buff3,"thread_3");
	sprintf(buff4,"thread_4");
	
	pthread_create(&a,NULL,threadAccount,NULL);
	pthread_create(&b,NULL,threadClient,buff1);
	pthread_create(&c,NULL,threadClient,buff2);
	pthread_create(&d,NULL,threadClient,buff3);
	pthread_create(&e,NULL,threadClient,buff4);
	
	pthread_join(a,NULL);
	pthread_join(b,NULL);
	pthread_join(c,NULL);
	pthread_join(d,NULL);
	pthread_join(e,NULL);
	
	
	
}
