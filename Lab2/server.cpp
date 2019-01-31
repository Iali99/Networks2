#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>

using namespace std;

void *threadMain(void * arg);
int sendMessage(int id, char message[1000]);

vector<int> clientAdresses;
int counter = 1;
struct ThreadArgs {
  int clientSocket;
  int clientID;
};

int main() {

  int sockid, n,  status, addr_len;
  pthread_t threadID;
  struct ThreadArgs *threadArgs;

  scanf("%d\n",&n );
  unsigned short port = 3545;
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
  printf("Waiting for connections!!\n");

  while(1) {
    //Accept the connection
    int socket_client = accept(sockid, (struct sockaddr *)&address, (socklen_t *)&addr_len);
    if (socket_client < 0) {
      printf("Failed to accept the connection request\n" );
      return 0;
    }
    if(counter > n) {
      char mess[1000] = "Cannot connect. Buffer full";
      send(socket_client, mess, sizeof(mess), 0);
      close(socket_client);
      continue;
    }
    clientAdresses.push_back(socket_client);
    printf("New client connected!\n");
    /* Create separate memory for client argument */
    if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL ) return 0;
    threadArgs -> clientSocket = socket_client;
    threadArgs -> clientID = counter++;
    //Create a client thread
    if (pthread_create (&threadID, NULL, threadMain, (void *) threadArgs) != 0) return 0;
  }


}

void *threadMain(void * args) {
  int clientSocket, clientID;
  pthread_detach(pthread_self());

  clientSocket = ((struct ThreadArgs *) args) -> clientSocket;
  clientID = ((struct ThreadArgs *) args) -> clientID;

  free(args);

  char buffer[1000] = {0};
  int count;
  while(1) {
    count = recv(clientSocket, buffer, 1000, 0);
    if(count == -1) {
      perror("Failed to read the message\n" );
      return 0;
    }
    if(strcmp(buffer,"exit") == 0) {
      close(clientSocket);
      clientAdresses.erase(clientAdresses.begin() + clientID - 1);
      string s = "One client left the conversation";
      strcpy(buffer, s.c_str());
      counter--;
      printf("One client left the conversation\n" );

    }
    sendMessage(clientID, buffer);

  }

}

int sendMessage(int id, char message[1000]) {
  int count;
  string s = "Client ";
  string s2(message);
  s.append(to_string(id));
  s.append(" : ");
  s.append(s2);
  char mess[1000];
  strcpy(mess, s.c_str());
for(int i =0;i<clientAdresses.size();i++) {
    count = send(clientAdresses[i], mess, sizeof(mess), 0);
    if(count == -1) {
      printf("Failed to send the message\n");
      return 0;
    }
  }
  return 1;
}
