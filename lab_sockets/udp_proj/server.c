#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

#define DEFAULT_PORT 8080
#define BUFSIZE 1000
#define DEFAULT_MAX_CLIENT_COUNT 50
#define DEBUG 0
#define DEBUG_PING 0
//
//	argv[1] - server port
//	argv[2] - max clients count
//
//
// long timers[50];
struct timeval timers_start[50];
char cl_names[50][50];
struct sockaddr_in cl_addr[50];
int connected_count=0;
sem_t semaphore;

char is_connected (char* name);
char* my_strsep(char** buf, char separator);
void reset_timer(int index);
void print_timers();
float timedifference_millis (struct timeval t0, struct timeval t1);
void* thread_disconnector();

int main (int argc, char* argv[])
{
	int serv_port = DEFAULT_PORT;
	int max_client_count = DEFAULT_MAX_CLIENT_COUNT;

	struct sockaddr_in serv_addr;
	struct sockaddr_in recv_addr;
	
	socklen_t addr_len = sizeof (cl_addr);
	int sock_fd;
	char buffer[BUFSIZE];
	pthread_t disconnector_t;

	int i;
	int j;
	char* name = 0;
	char* message = 0;

	for (i=0;i<50;++i)
		gettimeofday(&timers_start[i],NULL);

	if (argc >= 2)
		serv_port = atoi(argv[1]);
	if (argc >= 3)
		max_client_count = atoi(argv[2]);


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
	i=sem_init(&semaphore,0,1);
	if (DEBUG == 1)
		printf("DEBUG main: sem_init = %d\n",i);

	pthread_create(&disconnector_t,NULL,thread_disconnector,NULL);


	while (1)
	{
		memset ((char*)buffer,0,BUFSIZE);
		recvfrom(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*)&recv_addr, &addr_len);

		if (name != 0)
			free(name);
		if (message != 0)
			free(message);

		name = (char*)malloc(strlen(buffer));
		memset((char*)name,0,strlen(buffer));
		message = (char*)malloc(strlen(buffer));
		memset((char*)message,0,strlen(buffer));

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
		sem_wait(&semaphore);
		if (!is_connected (name))
		{
			if (connected_count >= max_client_count)
			{
				sprintf(buffer, "Connection refused, to many clients");
				sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&recv_addr, addr_len);
				continue;
			}
			else
			{
				memcpy(&cl_addr[connected_count], &recv_addr, sizeof(recv_addr));
				strcpy(cl_names[connected_count], name);
				gettimeofday(&timers_start[connected_count],NULL);
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
		sem_post(&semaphore);
	}
	return 0;



}
char is_connected (char* name)
{
	int i;
	if (DEBUG == 1)
		printf("DEBUG  is_connected: begin\n");
	print_timers();
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

void reset_timer(int index)
{
	gettimeofday(&timers_start[index], NULL);
}

void print_timers()
{
	int i=0;
	struct timeval stop;
	gettimeofday(&stop,NULL);printf("\n");
	for (i=0;i<connected_count;++i)
		printf("user: %s time: %f ms\n",cl_names[i],timedifference_millis(timers_start[i],stop));
}
float get_timer_value(int index)
{
	struct timeval stop;
	gettimeofday(&stop,NULL);
	return timedifference_millis(timers_start[index],stop);	
}

float timedifference_millis (struct timeval t0, struct timeval t1)
{
	return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec)/1000.f;
}

void* thread_disconnector()
{
	if (DEBUG == 1)
		printf("DEBUG thread_disconnector: begin\n");
	while (1)
	{
		int i=0;
		for (;i<connected_count;++i)
		{
			if (DEBUG==1)
				printf("DEBUG thread_disconnector: time: %f\n",get_timer_value(i));
			if (get_timer_value(i) > 10000.0f)
			{
				if (DEBUG == 1)
				{	
					printf("DEBUG thread_disconnector: client %d doesnt response\n",i);
					print_timers();
				}

				sem_wait(&semaphore);
				// przesunac cl_names
				memcpy(cl_names+i, cl_names+i+1,sizeof(cl_names[i])*(connected_count-i-1));
				// przesunac cl_addr
				memcpy(cl_addr+i, cl_addr+i+1,sizeof(cl_addr[i])*(connected_count-i-1));
				// przesunac timery
				memcpy(timers_start+i, timers_start+i+1,sizeof(timers_start[i])*(connected_count-i-1));
				--connected_count;
				sem_post(&semaphore);
				if (DEBUG == 1)
				{
					printf("DEBUG thread_disconnector: client %d shifting finished\n",i);
					print_timers();
				}
			}
		}
		sleep(1);
	}
}

