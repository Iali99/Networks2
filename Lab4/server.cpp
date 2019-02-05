/************* UDP SERVER CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

#define LEN 500
//Header Structure
struct Header {
    int sequenceNo;
    int Ack;
    int AckNo;
    int checksum;
};

struct Packet {
  struct Header header;
  char data[LEN];
};

int getChecksum(char * a);

int main(){
  int udpSocket, nBytes;
  char buffer[LEN];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;
  Packet sent,received;
  ofstream file;
  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  char filename[1024];
  printf("Enter the filename to put the contents in\n" );
  gets(filename);
  // struct timeval tv;
	// tv.tv_sec = 2;
	// tv.tv_usec = 50000;
	// if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
	// 	cout << "Failed to set socket timeout option" << endl;
	// 	return 0;
	// }
  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;
  file.open(filename,ios::binary | ios::out);
  bool flag = false;
  int expectedSeqNo = 1;
  while(1){
    /* Try to receive any incoming UDP datagram. Address and port of
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,(Packet *)&received,LEN + 16,0,(struct sockaddr *)&serverStorage, &addr_size);

    //cout << "DATA : ";

    // fwrite(received.data,1,sizeof(received.data),file);
  //  cout << endl;
    if(expectedSeqNo == received.header.sequenceNo && getChecksum(received.data) != received.header.checksum) {
      sent.header.Ack = 0;
      sent.header.AckNo = received.header.sequenceNo;
      strcpy(sent.data,"Data Corrupted. Please Send the packet again");
    }
    else {
      if(expectedSeqNo == received.header.sequenceNo){

        for(i=0;i<strlen(received.data);i++){
          printf("len = %d\n",strlen(received.data) );
          file << received.data[i] << std::flush;
        }
        if(nBytes < LEN)
          flag = true;
        expectedSeqNo += 1;
      }
      sent.header.Ack = 1;
      sent.header.AckNo = received.header.sequenceNo + 1;
      strcpy(sent.data,"Data Received");
    }
    nBytes = strlen(buffer) + 17;
    /*Send uppercase message back to client, using serverStorage as the address*/
    sendto(udpSocket,(Packet *)&sent,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
    if(flag)
      break;
  }
  // file.close();

  return 0;
}

int getChecksum(char * a){
  // printf("In getChecksum\n" );
  int result = 0;
  int i = 0;
  while(a[i] != 0 && i < LEN) {
    result ^= a[i];
    i++;
  }
  // printf("returning from checksum : %d\n",result );
  return result;
}
