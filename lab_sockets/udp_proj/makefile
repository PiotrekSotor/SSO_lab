all: client server
client: client.c
	gcc -o client client.c -Wall -lpthread
server: server.c
	gcc -o server server.c -Wall -lpthread -lrt