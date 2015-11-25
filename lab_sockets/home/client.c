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
	argv[3] = client name

*/
int main(int argc, char* argv[])
{

	struct sockaddr_in serv_addr; // adres servera
	struct hostent *server;
	int sock_fd;
	int serv_port;
	char buffer[255];
	int status;
	
	memset((char*)buffer, '\0',sizeof(buffer));
	
	if (argc < 4)
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

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		perror("gethostname function");
		return 1;
	}

	memset((char*)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("connect function");
		return 1;
	}
	
	printf("\tConnection established\n");
	printf("strcmp: %d\n",strcmp(buffer, "q"));
	// if (strcmp(buffer, "q"))
	// {
	// 	printf("strcmp(buffer, \"q\") yes\n");
	// }
	// else
	// {
	// 	printf("strcmp(buffer, \"q\") no\n");
	// }
	while (strcmp(buffer, "q")!=0)
	{
		printf("say something\n");
		scanf("%s",buffer);
		sprintf(buffer,"%s\n",buffer);
		status = write(sock_fd, buffer,strlen(buffer));
		if (status < 0)
		{
			perror("write function");
		}
	}
	shutdown(sock_fd,2);


	return 0;


}