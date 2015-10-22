#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#include<termios.h>



int main(int argc, char* argv[])
{
	int fd;
	int command_number = 0;
	int command_index = 0;
	int i=0;
	if (argc < 2)
	{
		printf("wymaga arg - fifo path\n");
		exit(1);
	}
	char prompt[20];
	char* command[5];
	
	fd = open(argv[1],O_WRONLY);
	while (fd <1 )
	{
		if (mkfifo(argv[1],0666) != 0 )
		{
			perror("mkfifo");
			exit(2);
		}
		fd = open(argv[1],O_WRONLY);	
	}
	for (i=0;i<5;++i)
		command[i] = (char*)malloc(15);
	
	while (1)
	{
		fgets (prompt, 20, stdin);
		i=0;
		command_number = 0;
		command_index = 0;
		
		/* split prompt to words */
		for (i=0;i<strlen(prompt);++i)
		{
			if (prompt[i] == ' ' || prompt[i] == '\n')
			{
				command[command_number][command_index] = '\0';;
				command_number++;
				command_index =0;
			}
			else
			{
				command[command_number][command_index]=prompt[i];
				command_index++;
			}
		}
		
		/* command parsing */
		if (strcmp(command[0], "pause") == 0)
		{
			sprintf (command[0],"pause\n");
			write(fd,"pause\n",(int)strlen(command[0]));
		}
		else if (strcmp(command[0],"seek") == 0)
		{
			sprintf (command[0],"seek %s\n",command[1]);
			write(fd, command[0],(int)strlen(command[0]));
		}
		else if (strcmp(command[0], "mute")==0)
		{
			sprintf(command[0],"mute %s\n",command[1]);
			write(fd,command[0],(int)strlen(command[0]));
		}
		else if (strcmp(command[0], "quit")==0)
		{
			sprintf(command[0],"quit\n");
			write(fd,command[0],(int)strlen(command[0]));
		}
		else if (strcmp(command[0], "stop")==0)
		{
			sprintf(command[0],"stop\n");
			write(fd,command[0],(int)strlen(command[0]));
		}
		else
			printf("unknown command\n");
		
		
	}
	close (fd);
	return 0;
}
