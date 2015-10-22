#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
	int i;
	int fd;
	int result;
	
	char buffer[BUF_SIZE];
	int mode;
	if (argc < 3)
		return -1;
	/*	
	//mode == 0 - otwiera do pisania, ale nie pisze
	//mode == 1 - otwiera do pisania, pisze co 1s
	//mode == 2 - otwiera do pisania, pisze nieprzerwanie
	*/
	mode = atoi(argv[2]);
	printf("Writer: fifo path: %s\n",argv[1]);
	

	fd = open(argv[1], O_WRONLY);
	if (fd < 0)
	{
		perror("open fail\n");
		return -3;
	}	
	printf("fd: %d",fd);
	if (mode != 0)
	{
		for (i=0;1;++i)
		{
			printf("Writer: %d\n",i);
			result = write(fd, buffer,BUF_SIZE);
		
			if (result == -1)
				perror("write");
			if (mode == 1)
				sleep(1);
		}
	}
	else 
		while(1);
	close(fd);
	return 0;
}
