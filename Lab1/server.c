#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){

  int sockid, status, addr_len;
  char buffer[1000] = {0};
  char mssg[1000];
  unsigned short port = 3544;
  struct sockaddr_in address;
  //Creating a Socket
  sockid = socket(PF_INET, SOCK_STREAM, 0);
  if(sockid == -1){
    printf("Socket Creation Failed\n");
    return 0;
  }

  //Setup the Address
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;
  addr_len = sizeof(address);
  //Binding the Socket to an Address
  status = bind(sockid,(struct sockaddr *)&address, addr_len );
  if(status == -1) {
    printf("Failed to bind the socket\n" );
    return 0;
  }

  //Listen for connection
  status = listen(sockid, 1);
  if(status == -1) {
    printf("Failed to Listen\n");
    return 0;
  }
  printf("Waiting for connection!!\n");
  //Accept the connection
  int socket_client = accept(sockid, (struct sockaddr *)&address, (socklen_t *)&addr_len);
  if (socket_client < 0) {
    printf("Failed to accept the connection request\n" );
    return 0;
  }
  printf("Got a connection. Starting service...\n");
  int count ;
  while(1) {
    count = recv(socket_client, buffer, 1000, 0);
    if(count == -1) {
      perror("Failed to read the message\n" );
      return 0;
    }

    printf("Client : " );
    puts(buffer);
    //Check to close the socket when "exit" is received
    if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't' && strlen(buffer) == 4) {
      //Closing a Socket
      close(socket_client);
      return 0;
    }
    printf("Send your message : ");
    gets(mssg);
    count = send(socket_client, mssg, sizeof(mssg), 0);
    if(count == -1) {
      printf("Failed to send the message\n");
      return 0;
    }
  }

  //Closing a Socket

}
