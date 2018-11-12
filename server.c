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

  server.sin_family = AF_INET;
  server.sin_port = htons(8096); //Define the port at which the serverer will listen for connections.
  server.sin_addr.s_addr = INADDR_ANY;

  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into socketFileDescriptor.

  // To handle errors, you can add an if condition that checks whether socketFileDescriptor is greater than 0. If it isn't, prompt an error

  bind(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server)); //assigns the address specified by server to the socket

  listen(socketFileDescriptor,5); //Listen for client connections. Maximum 5 connections will be permitted.

  //Now we start handling the connections.

  while(conn = accept(socketFileDescriptor, (struct sockaddr *)NULL, NULL)) {
      int pid;
      if((pid = fork()) == 0) {
          while (recv(conn, message, 100, 0)>0) {
              printf("Message Received: %s\n", message);
              //An extra breaking condition can be added here (to terminate the child process)
              //message = "";
          }
          exit(0);
      }
  }
}
