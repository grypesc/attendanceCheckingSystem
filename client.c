#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{

  struct sockaddr_in server; //This is our main socket variable.
  int socketFileDescriptor; //This is the socket file descriptor that will be used to identify the socket
  int conn; //This is the connection file descriptor that will be used to distinguish client connections.
  char message[100] = ""; //This array will store the messages that are sent by the serverer

  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_port = htons(8096);
//
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); //This binds the client to localhost

  connect(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server)); //This connects the client to the serverer.

  while(1) {
      printf("Enter a message: ");
      fgets(message, 100, stdin);
      send(socketFileDescriptor, message, strlen(message), 0);
      //An extra breaking condition can be added here (to terminate the while loop)
  }

}
