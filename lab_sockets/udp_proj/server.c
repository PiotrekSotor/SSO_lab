#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define DEFAULT_PORT 8080
#define BUFSIZE 1000
#define DEFAULT_MAX_CLIENT_COUNT 10
#define DEBUG 1
#define DEBUG_PING 0
//
//	argv[1] - server port
//	argv[2] - max clients count
//
//
long timers[50];
char cl_names[50][50];
int connected_count=0;

char is_connected (char* name);
char* my_strsep(char** buf, char separator);
void reset_timer(int index);
void increment_timers();

int main (int argc, char* argv[])
{
	int serv_port = DEFAULT_PORT;
	int max_client_count = DEFAULT_MAX_CLIENT_COUNT;

	struct sockaddr_in serv_addr;
	struct sockaddr_in recv_addr;
	struct sockaddr_in cl_addr[50];
	socklen_t addr_len = sizeof (cl_addr);
	int sock_fd;
	int recv_len;
	char buffer[BUFSIZE];


	int i;
	int j;
	char* name;
	char* message;

	for (i=0;i<50;++i)
		timers[i]=0;

	if (argc > 2)
		serv_port = atoi(argv[1]);
	if (argc > 3)
		max_client_count = atoi(argv[2]);

	// cl_addr = (struct sockaddr_in*) malloc(sizeof (struct sockaddr_in) * max_client_count);
	// cl_names = (char**) malloc(sizeof (char[50]) * max_client_count);
	// printf("cl_addr: %p, cl_names: %p\n",cl_addr, cl_names);
	// if (!cl_addr)
	// {
	// 	perror("cl_addr");
	// 	return -1;
	// }
	// if (!cl_names)
	// {
	// 	perror("cl_names");
	// 	return -1;
	// }

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket");
		return -1;
	}

	memset((char*)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(serv_port);

	if (bind (sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind failed");
		return 0;
	}
	printf("Server started on port %d\n",serv_port);

	while (1)
	{
		memset ((char*)buffer,0,BUFSIZE);
		recv_len = recvfrom(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*)&recv_addr, &addr_len);
		// printf("receiver data: %s | %d\n",buffer,recv_len);
		
		// name = strtok(buffer, ":");
		name = (char*)malloc(strlen(buffer));
		memset((char*)name,0,strlen(buffer));
		message = (char*)malloc(strlen(buffer));
		memset((char*)message,0,strlen(buffer));
		// name = my_strsep(&buffer, separator);
		// name = strsep(&buffer, delim);
		

		for (i=0;i<strlen(buffer);++i)
			if (buffer[i] != ':')
				name[i] = buffer[i];
			else
				break;

		++i;
		for (j=0;i<strlen(buffer);++i)
		{
			message[j++] = buffer[i];
		}
		
		if (DEBUG == 1)
		printf("DEBUG main: name %s : message: %s\n",name,message);
		if (!is_connected (name))
		{
			if (connected_count == max_client_count)
			{
				sprintf(buffer, "Connection refused, to many clients");
				sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&recv_addr, addr_len);
				break;
			}
			else
			{
				// printf("lol1\n");
				memcpy(&cl_addr[connected_count], &recv_addr, sizeof(recv_addr));
				// printf("lol2\n");
				strcpy(cl_names[connected_count], name);
				// printf("lol3\n");
				++connected_count;
				printf("Client %s connected\n",cl_names[connected_count-1]);
			}
		}
		if (strcmp(message,"PING")==0)
		{
			if (DEBUG_PING == 1)
				printf("DEBUG  main: odebrano PING\n");
			sprintf(buffer, "PONG");
			sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&recv_addr,addr_len);
			if (DEBUG_PING == 1)
				printf("DEBUG  main: wyslano PONG\n");
		}
		else
		{
			memset((char*)buffer,0,BUFSIZE);
			sprintf(buffer, "%s :: %s",name, message);
			if (DEBUG == 1)
				printf("DEBUG  main: wysylanie wiadomosci do wszsytkich\n\tmsg = %s | %d\n",buffer,connected_count);
			for (i=0;i<connected_count;++i)
			{
				sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&cl_addr[i],addr_len);
			}
		}
	}
	// close (sock_fd);
	return 0;



}
char is_connected (char* name)
{
	int i;
	if (DEBUG == 1)
		printf("DEBUG  is_connected: begin\n");
	increment_timers();
	for (i=0;i<connected_count; ++i)
	{
		if (DEBUG == 1)
			printf("DEBUG  is_connected: 1%s %s1\n",name, cl_names[i]);
		if (strcmp(name, cl_names[i]) == 0)
		{
			reset_timer(i);
			return 1;
		}
	}
	if (DEBUG == 1)
		printf("DEBUG  is_connected: end\n");
	return 0;
}

void increment_timers()
{
	int i;
	for (i=0;i<connected_count;++i)
		timers[i]++;
}

void reset_timer(int index)
{
	timers[index]=0;
}