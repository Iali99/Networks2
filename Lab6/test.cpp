#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <string>
// www.axmag.com/download/pdfurl-guide.pdf

using namespace std;

int main(int argc , char *argv[])
{
    int socket_desc;

    char *message;
    char server_reply[10000];
    char *filename = "file.jpg";
    int total_len = 0;
    int file_len = 6676;

    int len;

    FILE *file = NULL;
    struct sockaddr_in server;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("104.24.106.45");
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
    message = "GET /images/bjp_infographics.jpg HTTP/1.1\r\nHost: www.bjp.org\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n";

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
    // total_len += received_len;
    int c  = 0;
    while(1)
    {
        int received_len = recv(socket_desc, server_reply , sizeof server_reply , 0);

        if( received_len < 0 ){
            puts("recv failed");
            break;
        }

        total_len += received_len;
        int start = 0;
        if(c == 0){
          string str = string(server_reply);
          start = (int)str.find("\r\n\r\n");
          start += 4;
        }
        //puts(server_reply);
        // fwrite(server_reply , received_len , 1, file);
        while(start < received_len){
          fputc(server_reply[start],file);
          start ++;
        }

        printf("\nReceived byte size = %d\nTotal lenght = %d", received_len, total_len);

        if( received_len == 0){//total_len >= file_len ){
            break;
        }
        c++;
    }

    puts("Reply received\n");

    fclose(file);

    return 0;
}
