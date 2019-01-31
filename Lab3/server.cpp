#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct Message {
  string funcName;
  int n;
  float floatArgs[10];
};

void *threadMain(void * arg);

string getResult(string name,float floatArgs[10],int n);
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

  Message buffer;
  //char buffer[1000] = {0};
  int count;
  while(1) {
    count = recv(clientSocket, (struct Message *)&buffer, 1000, 0);
    if(count == -1) {
      perror("Failed to read the message\n" );
      return 0;
    }
    if(strcmp(buffer.funcName.c_str(),"exit") == 0) {
      close(clientSocket);
      clientAdresses.erase(clientAdresses.begin() + clientID - 1);
      counter--;
      printf("One client left the conversation\n" );
      pthread_cancel(pthread_self());

    }
    else {
    // sendMessage(clientID, buffer);
    printf("%s\n",(buffer.funcName).c_str() );
    string result = getResult((buffer.funcName).c_str(),buffer.floatArgs,buffer.n);
  //  printf("%f\n",buffer.floatArgs[0] );
    string s = "I got your message!";
    char mess[1000];
    strcpy(mess,result.c_str());
    count = send(clientSocket,mess,sizeof(mess),0);
  }

  }

}

string getResult(string name,float floatArgs[10],int n) {
  if(name.compare("sqrt") ==0) {
                    return to_string(sqrt(floatArgs[0]));

  }
  else if(name.compare("multiple") ==0) {
    string s = "";
                      s.append(to_string(floatArgs[0]*2));
                      s.append(", ");
                      s.append(to_string(floatArgs[0]*3));
                      return s;
    }
  else if(name.compare("add") ==0) {
    float sum = 0;
                for(int i = 0;i<n;i++){
                  sum+= floatArgs[0];
                }
                return to_string(sum);
    }
    return "Invalid Function Name";
}
// int sendMessage(int id, char message[1000]) {
//   int count;
//   string s = "Client ";
//   string s2(message);
//   s.append(to_string(id));
//   s.append(" : ");
//   s.append(s2);
//   char mess[1000];
//   strcpy(mess, s.c_str());
// for(int i =0;i<clientAdresses.size();i++) {
//     count = send(clientAdresses[i], mess, sizeof(mess), 0);
//     if(count == -1) {
//       printf("Failed to send the message\n");
//       return 0;
//     }
//   }
//   return 1;
// }
