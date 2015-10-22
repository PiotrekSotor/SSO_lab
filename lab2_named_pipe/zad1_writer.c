#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char* argv[])
{
	int i,j,k;
	int fd;
	int result;
	char string[20];
	if (argc < 2)
		return -1;
	printf("Writer: fifo path: %s\n",argv[1]);

	// result = mkfifo(argv[1],O_CREAT | S_IRUSR | S_IWUSR);
	// if (result < 0)
	// {
	// 	perror("mkfifo fail\n");
	// 	return -2;
	// }

	fd = open(argv[1], O_WRONLY);
	if (fd < 0)
	{
		perror("open fail\n");
		return -3;
	}	
	printf("fd: %d",fd);
	for (i=0;i<10;++i)
	{
		sprintf(string, "value: %d\n",i);
		// printf("%s %p\n",string,string);
		result = write(fd, string,strlen(string));
		//printf("	result: %d\n",result);
		if (result == -1)
			perror("write");
	}
	close(fd);

}