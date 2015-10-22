#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<fcntl.h>

int main(int argc, char** argv)
{
	char* prog_args[10][10];
	int pipefd[10][2];	
	int i;
	int j;
	int prog_count = 0;
	int arg_count = 0;
	char buff[50];
	int max_prog_count;
	int status;
	
	int pid_tab[10];
	printf("argc: %d\n",argc);
	for (i=0;i<10;++i)
		for (j=0;j<10;++j)
			prog_args[i][j] = (char*)malloc(10);	
	
	for (i=1;i<argc;++i)
	{
		if (strcmp(argv[i],"|")==0)
		{
			prog_args[prog_count][arg_count]=NULL;			
			prog_count++;
			arg_count=0;
		}
		else{
			// printf("try %d %s %d %d\n",i,argv[i],prog_count,arg_count);
			strcpy(prog_args[prog_count][arg_count++],argv[i]);
			// printf("ar: %s\n",prog_args[prog_count][arg_count-1]);
		}
			
	}
	
	prog_args[prog_count++][arg_count]=NULL;	
	max_prog_count = prog_count;
	// printf("max_prog_count: %d\n",max_prog_count);
	
	sleep(1);
	
	for (i=0;i<max_prog_count;++i)
	{

		pipe(pipefd[i]);
		pid_tab[i] = fork();

		

		if (pid_tab[i] == 0)
		{
			//warunek by pierwszy program mial zwykle standardowe wejscie
			if (i!=0)
			{
				dup2(pipefd[i-1][0],0);
				close(pipefd[i-1][1]);
			}
			else
			{
				close (pipefd[i-1][0]);	
				close (pipefd[i-1][1]);	
			}
			//warunek by ostatni program mial zwykle standardowe wyjscie	
			if (i!=max_prog_count-1)	
			{		
				dup2( pipefd[i][1],1);
				close(pipefd[i][0]);
			}
			else
			{
				// close (pipefd[i][0]);
				// close (pipefd[i][1]);
			}
			sprintf(buff, "LOL: %s %s %s\n",prog_args[i][0],prog_args[i][1],prog_args[i][2]);
			perror(buff);
			execvp(prog_args[i][0],prog_args[i]);
			exit(0);
		}
	}
	// prog_count = max_prog_count;
	// while (prog_count > 0)
	// {
	// 	i = wait(&status);
	// 	for (j=0;j<max_prog_count;++j)
	// 	{
	// 		if (i == pid_tab[j])
	// 		{
	// 			--prog_count;
	// 			break;
	// 		}
	// 	}
	// }

	// for (i=0;i<max_prog_count;++i)
	// {
	// 	while (wait(&status));
	// }

	// sleep(5);
	return 0;
}
