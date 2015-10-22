#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>

int main(int argc, char* argv[])
{
	int i,j,k;
	int fd;
	int result;
	char string[20];
	if (argc < 2)
		return -1;
	printf("Reader: fifo path: %s\n",argv[1]);

	// result = mkfifo(argv[1],0666 | O_CREAT);
	// if (result < 0)
	// {
	// 	perror("mkfifo fail");
	// 	exit(-2);
	// }
	fd = open(argv[1], O_RDONLY);
	printf("fd: %d\n",fd);
	if (fd < 0)
	{
		perror("open fail");
		exit (-3);
	}
	for (i=0;i<10;++i)
	{
		result = read(fd, string, 20);
		printf("Reader: %s",string);
	}
	close(fd);
	exit(0);
}