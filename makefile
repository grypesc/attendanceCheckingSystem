server: server.c client.c
	gcc client.c -o client
	gcc server.c -lpthread -o server
