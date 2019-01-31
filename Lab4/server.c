#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(){

  int sockid, status, addr_len;
  char buffer[1000] = {0};
  char mssg[1000];
  unsigned short port = 3544;
  struct sockaddr_in serverAddress, clientAddress;
  //Creating a Socket
  sockid = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockid == -1){
    printf("Socket Creation Failed\n");
    return 0;
  }

  //Setup the Address
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  addr_len = sizeof(serverAddress);
  //Binding the Socket to an Address
  status = bind(sockid,(struct sockaddr *)&serverAddress, addr_len );
  if(status == -1) {
    printf("Failed to bind the socket\n" );
    return 0;
  }
  printf("Server Started\n" );
  int count;
  socklen_t len;
  while(1) {
    count = recvfrom(sockid,(char *)buffer, 1000, 0, (struct sockaddr *)&clientAddress, (socklen_t *)&len);
    buffer[count] = '\0';

    printf("Client : " );
    puts(buffer);

    printf("Send your message : ");
    gets(mssg);
    printf("Sending message\n" );
    count = sendto(sockid, (const char *)mssg, 1000, 0, (const struct sockaddr *)&clientAddress, sizeof(clientAddress));
    printf("message sent\n" );
  }
  return 0;
  //Closing a Socket

}
