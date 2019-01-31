#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(){

  unsigned short serverPort = 3544;
  unsigned short clientPort = 5000;
  char buffer[1000] = {0};
  struct sockaddr_in serverAddress;
  //Setup the Address
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(serverPort);
  serverAddress.sin_addr.s_addr = INADDR_ANY;


  //Creating a Socket
  int sockid = socket(PF_INET, SOCK_DGRAM, 0);
  if(sockid == -1){
    printf("Socket Creation Failed\n");
    return 0;
  }
  // int status = bind(sockid,(struct sockaddr *)&clientAddress, sizeof(clientAddress) );
  // if(status == -1) {
  //   printf("Failed to bind the socket\n" );
  //   return 0;
  // }
  char mssg[1000];
  int count;
  socklen_t len;
  while(1) {
    printf("Enter the message to be sent : ");
    gets(mssg);

    sendto(sockid,(const char *)mssg, strlen(mssg),0,(const struct sockaddr *)&serverAddress,sizeof(serverAddress));

    count = recvfrom(sockid,(char *)buffer, 1000,0, NULL, NULL);
    //cout << buffer << endl;
    buffer[count] = '\0';
    printf("Server : " );
    puts(buffer);
    //cout << endl;
  }


}
