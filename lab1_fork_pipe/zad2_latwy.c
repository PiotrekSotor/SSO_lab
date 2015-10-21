#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>

int main(int argc, char** argv)
{
	//char* prog_args[10][10];
	//int pipefd[10][2];	
	int pipefd[2];
	char *prog1_args[10];
	char *prog2_args[10];
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
			prog1_args[arg_count++]=NULL;			
			break;
		}
		else{
			printf("try %d\n",i);
			strcpy(prog1_args[arg_count++],argv[i]);
		}
			
	}
	arg_count=0;
	for (;i<argc;++i)
	{
		printf("try %d\n",i);
		strcpy(prog2_args[arg_count++],argv[i]);	
	}
	pipe(pipefd);
	
	if (fork()==0)
	{
		//	prog1
		arg_count = 0;
		while (prog1_args[arg_count])
			printf("prog1: %s\n",prog1_args[arg_count++]);
		dup2(pipefd[1],1);
		
		execvp(prog1_args[0],prog1_args);
		exit(0);
	}
	if (fork()==0)
	{
		arg_count = 0;
		while (prog2_args[arg_count])
			printf("prog2: %s\n",prog2_args[arg_count++]);
		//	prog2
		dup2(pipefd[0],0);
		execvp(prog2_args[0],prog2_args);
		exit(0);
	}	



	sleep(5);
	return 0;
}
