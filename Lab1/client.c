#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){

  unsigned short port = 3544;
  char buffer[1000] = {0};
  struct sockaddr_in address;
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
  char mssg[1000];
  int count;
  while(1) {
    printf("Enter the message to be sent : ");
    gets(mssg);

    count = send(sockid, mssg, sizeof(mssg), 0);
    if(count == -1) {
      printf("Failed to send the message\n");
      return 0;
    }
    //Check to close the socket when "exit" is sent
    if(mssg[0] == 'e' && mssg[1] == 'x' && mssg[2] == 'i' && mssg[3] == 't' && strlen(mssg) == 4) {
      //Closing a Socket
      close(sockid);
      return 0;
    }
    count = recv(sockid, buffer, 1000, 0);
    if(count == -1) {
      printf("Failed to read the message\n" );
      return 0;
    }
    printf("Server : " );
    puts(buffer);
  }


}
