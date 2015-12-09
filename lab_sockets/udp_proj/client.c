#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>


#define DEBUG 0
#define DEBUG_PING 0
#define DEFAULT_PORT 8080
#define BUFSIZE 1000


struct sockaddr_in serv_addr;
socklen_t addr_len = sizeof (serv_addr);
int sock_fd;

char server_connected_flag = 0;
char received_pong = 0;
char username[50];


//protokół transmisji:
// co 500 ms wysyła PONG do servera, (osobny wątek?)
// server odpowiada w czasie do 500 ms, jesli nie to koniec


void *threadPong();

void *threadReceiver();

float timedifference_millis(struct timeval t0, struct timeval t1);




//
//	argv[1] - server addres
//	argv[2] - server port
//	argv[3] - nickname
//
int main (int argc, char* argv[])
{
	int serv_port = DEFAULT_PORT;
	struct hostent *serv_information;
	
	// int recv_len;
	char buffer[BUFSIZE];
	char sendbuffer[BUFSIZE];
	// int i;
	pthread_t pong_t;
	pthread_t receiver_t;
	

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
	strcpy(username,argv[3]);

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

	printf("Client started\n");
	server_connected_flag = 1;

	pthread_create(&pong_t,NULL,threadPong,NULL);
	pthread_create(&receiver_t,NULL,threadReceiver,NULL);


	while (1)
	{
		if (server_connected_flag == 0 )
		{
			if (DEBUG == 1)
				printf("DEBUG main: czekanie na polaczenie\n");
			sleep(1);
			continue;	
		}
		memset ((char*)buffer,0,BUFSIZE);
		read(0,buffer,BUFSIZE);
		sprintf(sendbuffer,"%s:%s",username,buffer);
		sendto(sock_fd, sendbuffer, strlen(sendbuffer), 0, (struct sockaddr*)&serv_addr, addr_len);
		if (DEBUG == 1)
			printf("DEBUG main: wysłano pakiet\n\t%s\n",sendbuffer);
	}

	return 0;
}

void *threadPong()
{
	struct timeval start, stop;
	char alarm_displayed = 0;
	char succes_displayed = 1;
	char buffer[10];
	sprintf(buffer,"%s:PING",username);

	if (DEBUG_PING == 1)
		printf("DEBUG threadPong: start threadPong\n");

	while (1)
	{
		sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, addr_len);
		if (DEBUG_PING == 1)
			printf("DEBUG threadPong: wysłano PING\n");
		gettimeofday(&start, NULL);
		while (received_pong == 0)
		{
			gettimeofday(&stop, NULL);
			if (timedifference_millis(start,stop) > 1000.0f)
			{
				if (DEBUG_PING == 1)
					printf("DEBUG threadPong: przekroczenie timooutu na PONG\n");
				received_pong = 0;
				server_connected_flag = 0;
				// Komunikat tylko przy pierwszym niepowodzeniu
				if (alarm_displayed == 0)
				{
					printf("\tServer doesnt respond\n");
					alarm_displayed = 1;
					succes_displayed = 0;
				}
				break;
			}
			usleep(100000);
		}
		// otrzymano odpowiedz na ping w spodziewanym czasie
		if (received_pong == 1)
		{
			server_connected_flag = 1;
			alarm_displayed = 0;
			if (succes_displayed == 0)
				printf("\tServer respond ok\n");
			succes_displayed = 1;
			if (DEBUG_PING == 1)
				printf("DEBUG threadPong: poprawny odbior PONG\n");
		}
		sleep(5);
		received_pong = 0;
	}
}

void *threadReceiver()
{
	char buffer[BUFSIZE];
	if (DEBUG == 1)
		printf("DEBUG threadReceiver: start threadReceiver\n");
	while (1)
	{
		memset ((char*)buffer,0,BUFSIZE);
		recvfrom(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*)&serv_addr, &addr_len);
		if (DEBUG == 1)
			printf("DEBUG threadReceiver: odebrano pakiet\n\t%s\n",buffer);
		if (strcmp(buffer,"PONG")==0)
		{
			if (DEBUG_PING == 1)
				printf("DEBUG threadReceiver: odebrano PONG\n");	
			received_pong = 1;
		}
		else
		{
			if (DEBUG == 1)
				printf("DEBUG threadReceiver: odebrano pakiet zwykly\n");
			printf("\t%s",buffer);
		}
	}

}

float timedifference_millis (struct timeval t0, struct timeval t1)
{
	return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec)/1000.f;
}