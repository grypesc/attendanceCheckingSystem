#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXNAME 30


int checkName(char data[])
{
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


int main()
{

  struct sockaddr_in server; //This is our main socket variable.
  int socketFileDescriptor; //This is the socket file descriptor that will be used to identify the socket
  int conn; //This is the connection file descriptor that will be used to distinguish client connections.
  char message[100] = ""; //This array will store the messages that are sent by the serverer
  memset(message, '\0', 100);
	char server_reply[2000];
  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_port = htons(8096);
//
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); //This binds the client to localhost

  if (connect(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server))!=0) { //This connects the client to the serverer.
    puts("Failed to connect to server");
    exit(0);
  }

      printf("Enter your name and a surname: ");
      fgets(message, 100, stdin);
      while (checkName(message) != 0 )
        puts ("Enter your name, space and then surname");

      send(socketFileDescriptor, message, strlen(message), 0);
      if( recv(socketFileDescriptor , server_reply , 2000 , 0) < 0)
      {
        puts("Your attendance check failed. Please try again or contact professor.");
        exit(1);
      }
      puts(server_reply);
      //An extra breaking condition can be added here (to terminate the while loop)


}
