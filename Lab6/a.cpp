#include<stdio.h>
#include <iostream>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <string>

using namespace std;

int getFileLength(string str,char * server_reply);
int main(int argc , char *argv[])
{
    int socket_desc;

    char *message;
    char server_reply[10000];
    char *filename = "file2.png";
    int received_bytes = 0;
    int file_len = 99999999;

    int len;

    FILE *file = NULL;
    struct sockaddr_in server;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("164.100.117.46");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }

    puts("Connected\n");

    //Send request
    message = "GET /images/logo_1.png HTTP/1.1\r\nHost: commerce.gov.in\r\n\r\n";

    if( send(socket_desc , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    puts("Data Send\n");

    remove(filename);
    file = fopen(filename, "ab");

    if(file == NULL){
        printf("File could not opened");
    }
    // int received_len = recv(socket_desc, server_reply , sizeof server_reply , 0);
    //
    // if( received_len < 0 ){
    //     puts("recv failed");
    //     // break;
    // }
    //
    // received_bytes += received_len;
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
          cout << "file length  = " << file_len << endl;
          start = (int)str.find("\r\n\r\n");
          start += 4;

        }
        //puts(server_reply);
        // fwrite(server_reply , received_len , 1, file);
        cout << server_reply;
        while(start < received_len){
          fputc(server_reply[start],file);
          start ++;
        }




        c++;
    }
    cout << endl;

    puts("Reply received\n");

    fclose(file);

    return 0;
}

int getFileLength(string str,char* server_reply) {
  int file_len = 0;
  int content_lenght = (int)str.find("Content-Length:");
  content_lenght += (int)sizeof("Content-Length:");
  while(isdigit(server_reply[content_lenght])){
    file_len = file_len*10 + server_reply[content_lenght] - '0';
    content_lenght ++;
  }
  return file_len;
}
