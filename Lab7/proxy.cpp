#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>
// #include <bits/stdc++.h>
#include <vector>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <map>
#include <chrono>

using namespace std;

struct ThreadArgs{
  int client;
};

int socketClient, addr_len;
vector<int> clientAdresses;
map<string, vector<string> > cache;
unsigned short port;
struct sockaddr_in address;

string getHost(string str);
int getFileLength(string str, char server_reply[10000]);
void * threadFunction(void * args);

int main(){


  socketClient = socket(PF_INET, SOCK_STREAM, 0);
  port = 4034;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;
  addr_len = sizeof(address);

  printf("%d\n", bind(socketClient, (struct sockaddr *)&address, addr_len));

  listen(socketClient, 1);
  printf("waiting for connection\n" );

  struct ThreadArgs * threadArgs;
  pthread_t threadID;

  while(1){
    int client = accept(socketClient, (struct sockaddr *)&address,(socklen_t *)&addr_len);
    printf("new client connected\n");
    clientAdresses.push_back(client);

    if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL ) return 0;
    threadArgs -> client = client;

    pthread_create(&threadID,NULL, threadFunction, (void *)threadArgs);
  }



  return 0;
}

void * threadFunction(void * args){
  int client = ((struct ThreadArgs *) args) -> client;
  int count;
  char clientMessage[1000];
  while(1){

    count = recv(client,clientMessage,1000,0);

    printf("Client Request : %s\n",clientMessage );

    string str(clientMessage);
    if(cache.find(str) != cache.end()){
      printf("Accessing Cached Item\n");
      vector<string> temp = cache[str];
      for(int i =0;i<temp.size();i++){
        if(send(client,temp[i].c_str(),temp[i].size(),0) < 0){
          printf("Unable to send\n" );
          break;
        }
      }
    }
    else{
    // ////
      string host = getHost(str);

      struct sockaddr_in server;

      struct hostent* h = gethostbyname2(host.c_str(),AF_INET);
      char * buf = inet_ntoa(*((struct in_addr*)h->h_addr));

      bzero(&server,sizeof(server));
      server.sin_addr.s_addr = inet_addr(buf);
      server.sin_family = AF_INET;
      server.sin_port = htons( 80 );

      int serverSocket;
      char server_reply[10000];
      string filename;
      int received_bytes = 0;
      int file_len = 99999999;

      int len;

      FILE *file = NULL;


      //Create socket
      serverSocket = socket(AF_INET , SOCK_STREAM , 0);
      if (serverSocket == -1)
      {
          printf("Could not create socket");
      }

      //Connect to remote server
      if (connect(serverSocket , (struct sockaddr *)&server , sizeof(server)) < 0)
      {
          puts("connect error");
          return 0;
      }

      // puts("Connected\n");

      if( send(serverSocket , clientMessage , strlen(clientMessage) , 0) < 0)
      {
          puts("Send failed");
          return 0;
      }

      // puts("Data Send\n");

      // size_t found = str.find_last_of("/\\");
      filename = "file.jpg";
      printf("Downloading %s\n",filename.c_str() );
      file = fopen(filename.c_str(), "ab");

      if(file == NULL){
          printf("File could not opened");
      }

      int c  = 0;
      while(received_bytes < file_len)
      {
          memset(server_reply,0,10000);
          int received_len = recv(serverSocket, server_reply , sizeof server_reply , 0);

          if( received_len < 0 ){
              puts("recv failed");
              break;
          }
          if(send(client,server_reply,received_len,0) < 0){
            printf("Unable to send\n" );
            break;
          }

          string t(server_reply);
          cache[str].push_back(t);

          received_bytes += received_len;
          int start = 0;
          if(c == 0){
            string str = string(server_reply);
            file_len = getFileLength(str,server_reply);
            // cout << "file length  = " << file_len << endl;
            start = (int)str.find("\r\n\r\n");
            start += 4;

          }
          while(start < received_len){
            fputc(server_reply[start],file);
            start ++;
          }

          c++;
      }

      fclose(file);

      close(serverSocket);
    }
  }
}
string getHost(string str){
  int pos = str.find("Host:");
  pos += sizeof("Host:");
  char req[1000];
  strcpy(req,str.c_str());
  string host = "";
  while(req[pos] != '\r'){
    string s(1,req[pos]);
    host.append(s);
    pos++;
  }
  return host;
}

int getFileLength(string str, char server_reply[10000]) {
  int file_len = 0;
  int content_lenght = (int)str.find("Content-Length:");
  content_lenght += (int)sizeof("Content-Length:");
  while(isdigit(server_reply[content_lenght])){
    file_len = file_len*10 + server_reply[content_lenght] - '0';
    content_lenght ++;
  }
  return file_len;
}
