#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>

void * senderThread(void * args);
void * receiverThread(void * args);

struct ThreadArgs {
  int socketID;
};

int main(){

  unsigned short port = 3545;
  char buffer[1000] = {0};
  struct sockaddr_in address;
  pthread_t threadID;
  struct ThreadArgs *threadArgs;
  //Setup the Address
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  //Creating a Socket
  int sockid = socket(PF_INET, SOCK_STREAM, 0);
  if(sockid == -1){
    printf("Socket Creation Failed\n");
    return 0;
  }

  //Connect to server
  int status = connect(sockid , (struct sockaddr *)&address, sizeof(address));
  if(status == -1) {
    printf("Failed to setup the connection\n");
    return 0;
  }
  printf("Connected to server\n");

  if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL) return 0;
  threadArgs -> socketID = sockid;
  if (pthread_create (&threadID, NULL, senderThread, (void *) threadArgs) != 0) return 0;
  if (pthread_create (&threadID, NULL, receiverThread, (void *) threadArgs) != 0) return 0;
  pthread_join(threadID, NULL);


}

void *senderThread(void * args) {
  char mssg[1000];
  int count;
  int sockid = ((struct ThreadArgs *) args) -> socketID;
  free(args);
  while(1) {
    // printf("Enter the message to be sent : ");
    gets(mssg);
    count = send(sockid, mssg, sizeof(mssg), 0);
    if(count == -1) {
      printf("Failed to send the message\n");
      return 0;
    }
    if(strcmp(mssg,"exit") == 0) {
      close(sockid);
      exit(1);
    }
    //printf("Message sent\n");
  }
}

void *receiverThread(void * args) {
  char buffer[1000];
  int count;
  int sockid = ((struct ThreadArgs *) args) -> socketID;
  while(1) {
    count = recv(sockid, buffer, 1000, 0);
    if(count == -1) {
      printf("Failed to read the message\n" );
      return 0;
    }
    printf("Message received\n" );
    puts(buffer);
    if(strcmp(buffer,"Cannot connect. Buffer full") == 0){
      close(sockid);
      exit(1);
    }
  }
}
