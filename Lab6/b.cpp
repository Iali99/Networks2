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
#include <chrono>

using namespace std;

struct ThreadArgs {
  int ID;
};

/* Global Variables
*/
struct sockaddr_in server;
std::ifstream input_file;
string host;
mutex file_lock;
vector<string> threadLinks[6];

int getFileLength(string str, char server_reply[10000]);

void subpartB();
void subpartC();
void subpartD();
void *threadFunction(void *args);

int main(int argc , char *argv[])
{
    auto begin = std::chrono::steady_clock::now();
    subpartB();
    auto end = std::chrono::steady_clock::now();
    // std::cout << "Subpart B : Time taken to download the files = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
    begin = std::chrono::steady_clock::now();
    subpartC();
    end = std::chrono::steady_clock::now();
    // std::cout << "Subpart C : Time taken to download the files = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
    begin = std::chrono::steady_clock::now();
    subpartD();
    end = std::chrono::steady_clock::now();
    // std::cout << "Subpart D : Time taken to download the files = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
    return 0;
}

void subpartB() {
    std::ifstream input_file("links6.txt");
    std::string str;
    string host;
    getline(input_file, host);
    server.sin_addr.s_addr = inet_addr("164.100.117.46");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );
    while (std::getline(input_file, str))
    {
        string message = "GET ";
        message.append(str);
        message.append(" HTTP/1.1\r\nHost: ");
        message.append(host);
        message.append("\r\n\r\n");
        cout << str << endl;
        int socket_desc;

        char server_reply[10000];
        string filename;
        int received_bytes = 0;
        int file_len = 99999999;

        int len;

        FILE *file = NULL;


        //Create socket
        socket_desc = socket(AF_INET , SOCK_STREAM , 0);
        if (socket_desc == -1)
        {
            printf("Could not create socket");
        }

        //Connect to remote server
        if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            puts("connect error");
            return;
        }

        // puts("Connected\n");

        if( send(socket_desc , message.c_str() , message.length() , 0) < 0)
        {
            puts("Send failed");
            return ;
        }

        // puts("Data Send\n");

        size_t found = str.find_last_of("/\\");
        filename = str.substr(found+1);
        printf("Downloading %s\n",filename.c_str() );
        file = fopen(filename.c_str(), "ab");

        if(file == NULL){
            printf("File could not opened");
        }

        int c  = 0;
        while(received_bytes < file_len)
        {
            int received_len = recv(socket_desc, server_reply , sizeof server_reply , 0);

            if( received_len < 0 ){
                puts("recv failed");
                break;
            }

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

        close(socket_desc);
    }
}

void subpartC() {
    std::ifstream input_file("links6.txt");
    std::string str;
    string host;
    getline(input_file, host);
    server.sin_addr.s_addr = inet_addr("164.100.117.46");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );

    int socket_desc;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return;
    }

    puts("Connected\n");
    while (std::getline(input_file, str))
    {
        string message = "GET ";
        message.append(str);
        message.append(" HTTP/1.1\r\nHost: ");
        message.append(host);
        message.append("\r\n\r\n");
        cout << str << endl;


        char server_reply[10000];
        string filename;
        int received_bytes = 0;
        int file_len = 99999999;

        int len;

        FILE *file = NULL;




        if( send(socket_desc , message.c_str() , message.length() , 0) < 0)
        {
            puts("Send failed");
            return ;
        }


        size_t found = str.find_last_of("/\\");
        filename = str.substr(found+1);
        printf("Downloading %s\n",filename.c_str() );
        file = fopen(filename.c_str(), "ab");

        if(file == NULL){
            printf("File could not opened");
        }

        int c  = 0;
        while(received_bytes < file_len)
        {
            int received_len = recv(socket_desc, server_reply , sizeof server_reply , 0);

            if( received_len <= 0 ){
                // puts("recv failed");
                break;
            }

            received_bytes += received_len;
            int start = 0;
            if(c == 0){
              string str = string(server_reply);
              file_len = getFileLength(str,server_reply);
              cout << "file length  = " << file_len << endl;
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


    }
    close(socket_desc);
}

void subpartD(){
  std::ifstream input_file("links6.txt");
  getline(input_file, host);
  int c = 0;
  string str;
  while(getline(input_file,str)){
    threadLinks[c%6].push_back(str);
    c++;
  }
  pthread_t threads[6];
  struct ThreadArgs ** threadArgs = (ThreadArgs **)malloc(sizeof(ThreadArgs *)*6);
  for(int i = 0;i<6;i++){
    if ((threadArgs[i] = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL) return;
    threadArgs[i]->ID = i;
    pthread_create(&threads[i],NULL,threadFunction,(void *)threadArgs[i]);
  }
  for(int i =0;i<6;i++){
    pthread_join(threads[i],NULL);
  }
}

void *threadFunction(void * args){
  std::string str;
  int id = ((struct ThreadArgs *) args) -> ID;
  free(args);

  int total = threadLinks[id].size();
  server.sin_addr.s_addr = inet_addr("164.100.117.46");
  server.sin_family = AF_INET;
  server.sin_port = htons( 80 );

  int socket_desc;
  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
      printf("Could not create socket");
  }

  //Connect to remote server
  if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
      puts("connect error");
      return 0;
  }

  puts("Connected\n");
  for(int l = 0;l<total;l++)
  {
      str = threadLinks[id][l];
      string message = "GET ";
      message.append(str);
      message.append(" HTTP/1.1\r\nHost: ");
      message.append(host);
      message.append("\r\n\r\n");


      char server_reply[10000];
      string filename;
      int received_bytes = 0;
      int file_len = 99999999;

      int len;

      FILE *file = NULL;

      if( send(socket_desc , message.c_str() , message.length() , 0) < 0)
      {
          puts("Send failed");
          return 0;
      }
      // puts("Data Send\n");

      size_t found = str.find_last_of("/\\");
      filename = str.substr(found+1);
      printf("Downloading  %s\n", filename.c_str());
      file = fopen(filename.c_str(), "ab");

      if(file == NULL){
          printf("File could not opened");
      }

      int c  = 0;
      while(received_bytes < file_len)
      {

          int received_len = recv(socket_desc, server_reply , sizeof server_reply , 0);
          if( received_len <= 0 ){
              break;
          }

          received_bytes += received_len;
          int start = 0;
          if(c == 0){
            string str = string(server_reply);
            file_len = getFileLength(str,server_reply);
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


  }
  close(socket_desc);
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
