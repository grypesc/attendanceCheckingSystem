#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>

#define MAXNAME 30

struct Node {
  char name[30];
  char surname[30];
  struct Node *next;
};

  struct Node *head = NULL;


 void addToList(char data[]) {

  struct Node *new = malloc(sizeof(struct Node));


  char c = data[0];
  int i=0;
  while (c != ' ' && i<MAXNAME)
  {
    new->name[i]=c;
    i++;
    c=data[i];

  }
  new->name[i]='\0';
  i++;
  c=data[i];
  int j=0;
  while (c != '\0'  && j<MAXNAME)
  {
    new->surname[j]=c;
    i++;
    j++;
    c=data[i];
  }
  new->surname[j]=c;
  new->next = head;
  head = new;
}

void printList() {
  struct Node* current = head;
  int i=1;
  printf("no. Name                           Surname\n");
  while(current != NULL) {
    if(i<10)
      printf("0");
    printf("%d. %s",i, current->name);
    int len = strlen(current->name);
    for (len;len<=MAXNAME;len++)
      printf (" ");

    printf("%s", current->surname);
    current = current->next;
    i++;
  }
  printf("\n");
}

int saveList() {
  FILE *fd;
  fd = fopen("attendanceList.txt", "w");
  fprintf(fd, "no. Name                           Surname\n");
  int i=0;

  struct Node *current = head;
  int len = 0;
  while(current != NULL) {
    if(i<10)
      fprintf(fd, "0");
    fprintf(fd, "%d. %s",i, current->name);
    len = strlen(current->name);
    for (len;len<=MAXNAME;len++)
      fprintf (fd, " ");

    fprintf(fd, "%s", current->surname);
    current = current->next;
    i++;
  }
  fclose(fd);
}

int main()
{
  struct sockaddr_in server; //This is our main socket variable.
  int socketFileDescriptor; //This is the socket file descriptor that will be used to identify the socket
  int client_sock; //This is the connection file descriptor that will be used to distinguish client connections.
  char message[100] = ""; //This array will store the messages that are sent by the serverer
  char messageToClient[35] = "Your attendance check is completed";

  server.sin_family = AF_INET;
  server.sin_port = htons(8096); //Define the port at which the serverer will listen for connections.
  server.sin_addr.s_addr = INADDR_ANY;

  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into socketFileDescriptor.

  // To handle errors, you can add an if condition that checks whether socketFileDescriptor is greater than 0. If it isn't, prompt an error

  bind(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server)); //assigns the address specified by server to the socket

  listen(socketFileDescriptor,5); //Listen for client connections. Maximum 5 connections will be permitted.

  //Now we start handling the connections.
  struct sockaddr_in clientAddr;
  socklen_t slen = sizeof(clientAddr);

  while(client_sock = accept(socketFileDescriptor, (struct sockaddr *)&clientAddr, &slen )) {
        memset(message, '\0', 100);
        recv(client_sock, message, 100, 0);
        printf("Attendance request received from %s::%d, %s\n", inet_ntoa(clientAddr.sin_addr), (int) ntohs(clientAddr.sin_port), message);
        addToList(message);
        write(client_sock , messageToClient , strlen(messageToClient));
        //An extra breaking condition can be added here (to terminate the child process)
        //message = "";
             printList();

             saveList();
  }
}
