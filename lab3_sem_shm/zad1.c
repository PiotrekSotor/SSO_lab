#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define SEM_KEY_FILE key_file.key
#define SHM_SIZE 10

int main(int argc, char* argv[])
{
	//key_t sem_keys[5];
	key_t sem_key;
	key_t shm_key;
	int sem_id;
	struct sembuf sop;
	int shm_id;
	char* shm;
	int i;
	
	
	
	sem_key = ftok("./zad1.c",60);
	perror("ftok");
	sem_id = semget(sem_key, 1,0);
	if (sem_id == -1)
	{
		semctl(sem_id, 0 ,IPC_RMID, 0);
		perror("semctl IPCRMID");
		sem_id = semget(sem_key, 1 , IPC_CREAT | IPC_EXCL | 0666);
		perror("semget 2nd time");
		
	}
	perror("semget");
	semctl(sem_id, 0 ,SETVAL, 0);
	perror("semctl");
	
	shm_key = 1324;
	shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
	perror("shmget");
	shm = shmat(shm_id, NULL, 0);
	perror("shmat");
	printf("%p\n",shm);
	
	
	shm[9] = '\0';
	printf("%s\n",shm);
	i=0;
	
	if (fork() == 0)
	{
		
		while (1)
		{
			
			sop.sem_num = 0; // numer semafora w paczce semaforow tworzonej przez sem_get
			sop.sem_op = -1;
			sop.sem_flg = 0;
			
			semop(sem_id, &sop, 1);
			
			perror("proc 2 semop");
			printf("proc 2 shm: %s\n", shm);
			
			
		}
		return 0;
	}
	else
	{
		while (1)
		{
		
			sop.sem_num = 0; // numer semafora w paczce semaforow tworzonej przez sem_get
			sop.sem_op = 1;
			sop.sem_flg = 0;
			
			semop (sem_id, &sop, 1);
			perror("proc 1 semop");
			
			sprintf(shm,"cnt: %d", i++);
			
			sleep(1);
		}
		return 0;
	}
	
	
	
	
	
	/*
	for (i=0;i<5;++i)
	{
		sem_key = ftok("./zad1.c", 40+i);
	}*/
	
	
	
	
	
}

