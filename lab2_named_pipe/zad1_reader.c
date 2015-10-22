#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
	int i;
	int fd;
	int result;
	int mode;
	
	char buffer[BUF_SIZE];
	if (argc < 3)
		return -1;
	/*	
	mode == 0 - otwiera do czytania, ale nie czyta
	mode == 1 - otwiera do czytania, czyta co 1 s 
	mode == 2 - otwiera do czytania, czyta nieprzerwanie
	*/
	mode = atoi(argv[2]);
	
	
	
	printf("Reader: fifo path: %s\n",argv[1]);

	
	
	fd = open(argv[1], O_RDONLY);
	printf("fd: %d\n",fd);
	if (fd < 0)
	{
		perror("open fail");
		exit (-3);
	}
	if (mode != 0)
	{
		
		for (i=0;1;++i)
		{
			
			result = read(fd, buffer, BUF_SIZE);
			if (result == 0)
			{
				printf("cannot read\n");
				exit(1);
			}
				
			else
				printf("Reader: %d\n",i);			
			if (mode == 1)
				sleep(1);
		}
	}
	else
		while(1);
	close(fd);
	return 0;
}
