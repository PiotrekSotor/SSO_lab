#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#define BUF_SIZE 4096

int main(int argc, char** argv)
{
	int fd[2];
	int pid;
	char symbol[BUF_SIZE];
	int numOfLetters;
	int status;
	int clock;
	int counter =0;
	int i;
	srand(time(NULL));

	if (pipe(fd)==-1)
		return -1;
	
	pid = fork();

	

	//potomny odczytuje
	//macierzysty pisze
	if (pid == 0)
	{
		printf("Potomny: start\n");
		close(fd[1]);
		
		do{
			sleep(30);			
			for (i=0;i<50;++i)
			{	
				sleep(1);		
				if (status = read(fd[0],symbol,BUF_SIZE) <=0)
					printf("Potomny: read status: %d",status);
				printf("Potomny: %s\n",symbol);
			}
			
		}while (strcmp(symbol,"qq") != 0);
		close(fd[0]);		
		exit(0);		
	}
	else if (pid > 0)
	{
		printf("Macierzysty: start\n");
		close(fd[0]);
		do{
			if (argc == 2 && strcmp(argv[1],"a")==0)
			{
				
				sleep(10);
				for (status = 0;status<10;++status)
				{	
								
					sprintf(symbol,"counter: %d",counter++);
					write(fd[1],symbol,BUF_SIZE);
					printf("Macierzysty: generated: %s\n",symbol);
				}
			}
			else
			{			
				scanf("%s",symbol);
				//printf("Macierzysty: %s\n",symbol);
			}
			write(fd[1],symbol,BUF_SIZE);
			
			
		}while (strcmp(symbol,"qq") != 0);
		close(fd[1]);
	}
	else
		return -2;
	printf("Macierzysty: koniec\n");	
	while (wait(&status) != pid);
	printf("Macierzysty: child exit status: %d\n",status);
	return 0;
}
