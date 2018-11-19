#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXNAME 30


int checkName(char data[]) {
  char c = data[0];
  int i=0;
  while (c != ' ' && i<MAXNAME)
  {
    if (c == '\n') return 1;
    i++;
    c=data[i];
  }
  c=data[i];
  int j=0;
  while (c != '\0'  && j<MAXNAME)
  {
    i++;
    j++;
    c=data[i];
  }
  return 0;
}


int main(int argc, char *argv[]) {
  if (argc!=3) {
    puts ("Program parameters: <Server adress> <port number>");
    exit(1);
  }

  char* ipAdress = argv[1];
  int portNumber = atoi(argv[2]);

  struct sockaddr_in server;
  int socketFileDescriptor;
  int conn;
  char message[100] = "";
  memset(message, '\0', 100);
  char server_reply[2000];
  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_port = htons(portNumber);
  //
  inet_pton(AF_INET, ipAdress, &server.sin_addr); //Bind the client to server

  if (connect(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server))!=0) {
    puts("Failed to connect to server");
    exit(0);
  }

  do {
    puts ("Enter your name, space and then surname");
    fgets(message, 100, stdin);
  } while (checkName(message) != 0 );

  send(socketFileDescriptor, message, strlen(message), 0);
  if( recv(socketFileDescriptor , server_reply , 2000 , 0) < 0)
  {
    puts("Your attendance check failed. Please try again or contact professor.");
    exit(1);
  }
  puts(server_reply);
  return 0;
}
