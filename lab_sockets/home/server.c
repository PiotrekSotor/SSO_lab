#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 


/*
	argv[1] = server name
	argv[2] = server port
*/
int main(int argc, char* argv[])
{

	struct sockaddr_in serv_addr; // adres servera
	struct sockaddr_in c_addr; // adres clienta
	char buffer[255];
	int sock_fd;
	int new_sock_fd;
	int serv_port;
	socklen_t c_len;

	if (argc < 3)
	{
		printf("za malo argumentow\n");
		return 1;
	}

	serv_port = atoi(argv[2]);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0 )
	{
		perror("socket function");
		return 1;
	}

	memset((char*)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind (sock_fd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		perror("bind function");
		return 0;
	}
	listen(sock_fd,5);

	printf("\tConnection established\n");
	c_len = sizeof(c_addr);
	new_sock_fd = accept(sock_fd,(struct sockaddr*)&c_addr,&c_len);

	printf("after accept\n");
	memset((char*)&buffer, '\0',sizeof(buffer));
	read(new_sock_fd, buffer,255);
	printf("asd: %s\n",buffer);

	while (read(new_sock_fd, buffer,255) > 0)
	{
		printf("received: %s",buffer);
		memset((char*)&buffer, '\0',sizeof(buffer));
	}
	shutdown(new_sock_fd,2);

	printf("after while(1)\n");

	while(1);



	return 0;


}