#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

void * senderThread(void * args);
void * receiverThread(void * args);

struct ThreadArgs {
  int socketID;
};

struct Message {
  string funcName;
  int n;
  float floatArgs[10];
};

int main(){

  unsigned short port = 3544;
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

  while(1) {
  Message mssg;

  int count;
  string name;
    printf("Enter the Function Name : ");
    cin >> name;
    mssg.funcName = name;

    if(name.compare("sqrt") ==0) {
        printf("Enter the argument: ");
        mssg.n = 0;
        cin >> mssg.floatArgs[0];

    }
    else if(name.compare("multiple") ==0) {
          printf("Enter the argument: \n");
          mssg.n = 0;
          cin >> mssg.floatArgs[0];
      }
    else if(name.compare("add") ==0) {
      int n;
                   printf("Enter the number of arguments: ");
                   cin >> n;
                   mssg.n = n;
                   printf("Enter the arguments : ");
                   for(int i =0;i<n;i++) {
                     cin >> mssg.floatArgs[i];
                   }
      }
      printf("asdfasdfsad\n");
      if(name.compare("exit")) {
        mssg.floatArgs[0] = 0;
        mssg.n = 1;
      }

    count = send(sockid, (void *)&mssg, sizeof(mssg), 0);
    if(count == -1) {
      printf("Failed to send the message\n");
      return 0;
    }
    if(name.compare("exit") == 0) {
      close(sockid);
      exit(1);
    }
    // if(strcmp(mssg,"exit") == 0) {
    //   close(sockid);
    //   exit(1);
    // }
    printf("Message sent\n");
    //char buffer[1000] = {0};
    recv(sockid,buffer,1000,0);
    printf("%s\n",buffer );

    usleep(10000);
  }


}
