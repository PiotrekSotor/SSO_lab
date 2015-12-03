#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define DEFAULT_PORT 8080
#define BUFSIZE 1000
//
//	argv[1] - server addres
//	argv[2] - server port
//	argv[3] - nickname
//
int main (int argc, char* argv[])
{
	int serv_port = DEFAULT_PORT;
	struct sockaddr_in serv_addr;
	struct hostent *serv_information;
	socklen_t addr_len = sizeof (serv_addr);
	int sock_fd;
	int recv_len;
	char buffer[BUFSIZE];
	int i;

	if (argc < 4)
	{
		printf("Argument list:\n\t (server addres) (server port) (your chat nickname)\n");
		return -1;
	}

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket");
		return -1;
	}


	memset((char*)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);

	serv_information = gethostbyname(argv[1]);
	if (!serv_information)
	{
		printf("couldnt find server addres");
		return -1;
	}
	memcpy((void*)&serv_addr.sin_addr, serv_information->h_addr_list[0], serv_information->h_length);

	printf("Client started");

	for (i = 0; i<10; ++i)
	{
		sprintf(buffer, "%s:message: %i\n",argv[3],i);
		sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, addr_len);
		// recv_len = recvfrom(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*)&cl_addr, &addr_len);
		printf("sent data: %s\n",buffer);


	}
	// close (sock_fd);
	return 0;



}