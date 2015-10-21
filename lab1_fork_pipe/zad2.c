#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>

int main(int argc, char** argv)
{
	char* prog_args[10][10];
	int pipefd[10][2];	
	int i;
	int prog_count = 0;
	int arg_count = 0;
	int max_prog_count;
	
	int pid_tab[10];
	printf("argc: %d\n",argc);
	for (i=1;i<argc;++i)
	{
		if (strcmp(argv[i],"|")==0)
		{
			prog_args[prog_count][arg_count]=NULL;			
			prog_count++;
			arg_count=0;
		}
		else{
			printf("try %d %s %d %d\n",i,argv[i],prog_count,arg_count);
			strcpy(prog_args[prog_count][arg_count++],argv[i]);
			printf("ar: %s\n",prog_args[prog_count][arg_count-1]);
		}
			
	}
	printf("max_prog_count: %d\n",max_prog_count);
	prog_args[prog_count++][arg_count]=NULL;	
	max_prog_count = prog_count;
	
	
	sleep(1);
	for (i=0;i<max_prog_count;++i)
	{
		pid_tab[i] = fork();

		pipe(pipefd[i]);

		if (pid_tab[i] == 0)
		{
			//warunek by pierwszy program mial zwykle standardowe wejscie
			if (i!=0)
				dup2(pipefd[i-1][0],0);
			//warunek by ostatni program mial zwykle standardowe wyjsie	
			if (i!=max_prog_count-1)			
				dup2(pipefd[i][1],1);
			
			printf("LOL: %s %s\n",prog_args[i][0],prog_args[i][1]);
			execvp(prog_args[i][0],prog_args[i]);
			exit(0);
		}
	}
	sleep(5);
	return 0;
}
