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
#include <time.h>

#define SEM_KEY_FILE key_file.key
#define SHM_SIZE 6


int main(int argc, char* argv[])
{
	key_t sem_key;
	key_t shm_key;
	int sem_id;
	struct sembuf sop_lewy;
	struct sembuf sop_prawy;
	int shm_id;
	char* shm;
	int i;
	int rand_time;
	srand(time(NULL));
	long array[5] = {1,1,1,1,1};
	
	
	sem_key = ftok("./zad1_final.c",80);
	perror("ftok");
	sem_id = semget(sem_key, 1,0);
	if (sem_id == -1)
	{
		semctl(sem_id, 0 ,IPC_RMID, 0);
		perror("semctl IPCRMID");
		sem_id = semget(sem_key, 5 , IPC_CREAT | IPC_EXCL | 0666);
		perror("semget 2nd time");
		
	}
	printf("sem_id : %d\n",sem_id);
	if ( -1 == semctl(sem_id, 1,SETALL, 0));
		perror("semctl");
	
	shm_key = 1324;
	if ( -1 == (shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666)));
		perror("shmget");
	if (-1 == (shm = shmat(shm_id, NULL, 0)));
		perror("shmat");
	printf("%p\n",shm);
	
	
	sprintf(shm,"     ");
	
	// i-ty semafor - lewy widelec i-tego filozofa
	// (i+1)%5 semafor - prawy widelec i-tego filozofa
	
	for (i=0;i<5;++i)
	{
		if (fork() == 0)
		{
			sop_lewy.sem_num = i;
			sop_lewy.sem_flg = 0;
			
			sop_prawy.sem_num = (i+1)%5;
			sop_prawy.sem_flg = 0;
			printf("l sem: %d  p sem: %d\n",sop_lewy.sem_num,sop_prawy.sem_num);
			
			while (1)
			{
				shm[i] = '1';
				printf("%s\n",shm);
				sop_lewy.sem_op = -1;
				if (semop(sem_id, &sop_lewy, 1) == -1);
					printf("errno: %d %d\n",errno, sop_lewy.sem_num);
				
				
				shm[i] = '2';
				printf("%s\n",shm);
				sop_prawy.sem_op = -1;
				semop(sem_id, &sop_prawy, 1);
				
				shm[i] = 'J';
				printf("%s\n",shm);
				rand_time = rand()%5;
				usleep(rand_time * 100000 + 200000);
				
				shm[i] = '3';
				printf("%s\n",shm);
				sop_lewy.sem_op = 1;
				semop(sem_id, &sop_lewy, 1);
				
				shm[i] = '4';
				printf("%s\n",shm);
				sop_prawy.sem_op = 1;
				semop(sem_id, &sop_prawy, 1);
				
				shm[i] = 'M';
				printf("%s\n",shm);
				rand_time = rand()%20;
				usleep(rand_time * 100000 + 200000);
				usleep(500000);
				
				
			}
			
			return 0;
		}
		
		
		
	}
	while(1);
	
	
}
