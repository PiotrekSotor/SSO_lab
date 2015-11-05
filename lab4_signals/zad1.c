#include<signal.h>
#include<stdlib.h>
#include<setjmp.h>
#include<stdio.h>

int suscnt= 0; // CTRL + Z
int intcnt = 0; // CTRL + C

void int_handler(int signum)
{
	intcnt++;
	printf("SIGINT: %d",intcnt);
}


int main(int argc, char* argv[])
{

	int pid = fork();
	printf("pid: %d\n",pid);
	if (pid == 0)
	{
		printf("started\n");
		signal(SIGINT, int_handler);
		
		while (1)
			{
				printf("child: intcnt: %d\n", intcnt);
				sleep(1);	
			}
	}
	else
	{
		signal(SIGINT, SIG_IGN);
		while(1)
		{
		kill(pid,SIGINT);
		printf("parent: send kill\n");
		sleep(1);
		}


	}
}