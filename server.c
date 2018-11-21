#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#define MAXNAME 30
#define ACCEPTINGTIME 600

struct Node {
  char name[MAXNAME];
  char surname[MAXNAME];
  struct Node *next;
};

struct Node *head = NULL;
struct sockaddr_in clientAddr;
sem_t mutex;

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

void saveList() {
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
};

void alert() {
  puts("Time is up, attendance system is shutting down. Attendance list was saved.");
  saveList();
  exit(1);
}

void *threadBody(void *socket_desc) {

  int sock = *(int*)socket_desc;
  char message[100];

  recv(sock, message, 100, 0);
  if (strlen(message)<=5) {
    puts("Client prompted wrong input");
    strcpy(message, "Your attendance check FAILED");
    write(sock , message , strlen(message));
    pthread_exit(NULL);
  }
  printf("Attendance request received from %s::%d, name: %s\n", inet_ntoa(clientAddr.sin_addr), (int) ntohs(clientAddr.sin_port), message);
  sem_wait(&mutex);
  addToList(message);
  printList();
  sem_post(&mutex);

  memset(message, 0, 100);
  strcpy(message, "Your attendance check is succesful");
  write(sock , message , strlen(message));
  pthread_exit(NULL);
}

void handleInterruption(int signalType) {
  saveList();
  puts("\nAttendance list was saved, exiting server.");
  exit(0);
}


int main(int argc, char *argv[]) {
  if (argc!=2) {
    puts ("Program parameters: <Port number>");
    exit(1);
  }
  int portNumber = atoi(argv[1]);

  struct sigaction signalHandler;
  signalHandler.sa_handler = handleInterruption;
  sigfillset(&signalHandler.sa_mask);
  signalHandler.sa_flags = 0;
  if (sigaction(SIGINT, &signalHandler, 0) < 0) {
    puts ("Failed to SIGINT, line 112");
    exit(1);
  }


  struct sockaddr_in server;
  int socketFileDescriptor;
  int clientDescriptor;
  char message[100] = "";

  server.sin_family = AF_INET;
  server.sin_port = htons(portNumber);
  server.sin_addr.s_addr = INADDR_ANY;

  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &server.sin_addr, ip, INET_ADDRSTRLEN);

  printf("Attendance system is on, server address is %s with a port %d\n", ip, portNumber);

  srand(time(NULL));
  int sleepTime = rand()%5+5;
  printf("Attendance checking will begin in %d seconds\n", sleepTime);
  sleep(sleepTime);

  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

  bind(socketFileDescriptor, (struct sockaddr *)&server, sizeof(server));

  listen(socketFileDescriptor,5);

  socklen_t slen = sizeof(clientAddr);

  pthread_t thread_id;

  printf ("Attendance check has started and will last %d seconds\n", ACCEPTINGTIME);

  signal(SIGALRM, alert);
  alarm(ACCEPTINGTIME);
  sem_post(&mutex);
  while(clientDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&clientAddr, &slen )) {

    if( pthread_create( &thread_id , NULL ,  threadBody , (void*) &clientDescriptor) < 0)
    {
      perror("Failed to create a thread");
      return 1;
    }
  }
}
