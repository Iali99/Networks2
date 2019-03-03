/************* UDP SERVER CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#define LEN 100
#define WIN_SIZE 4
//Header Structure
struct DataPack {
    int type;
    int isLast;
    int sequenceNo;
    int checksum;
    int payloadLength;
    char payload[LEN];
};

struct Ack {
  int type;
  int recvSequenceNumber;
  int checksum;
};

int getChecksum(char * a);

vector<int> receivedPackets;
vector<DataPack> receivedBuffer;

int main(){
  int udpSocket, nBytes, rcvBase;
  rcvBase = 0;
  char buffer[LEN];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;

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
  Ack toSend;
  DataPack received;
  while(1){
    nBytes = recvfrom(udpSocket,(DataPack *)&received,LEN + 16,0,(struct sockaddr *)&serverStorage, &addr_size);
    int seqNo = received.sequenceNo;
    if(seqNo >= rcvBase && seqNo < rcvBase + WIN_SIZE) {
      toSend.type = 2;
      toSend.recvSequenceNumber = seqNo;
      toSend.checksum = getChecksum(received.payload);
      sendto(udpSocket,(Ack *)&toSend,12,0,(struct sockaddr *)&serverStorage,addr_size);
      receivedPackets.push_back(seqNo);
      receivedBuffer.push_back(received);
      printf("New Packet Recieved\n");
      if(seqNo == rcvBase) {
        do {
          for(int i = 0;i< receivedBuffer.size();i++){
            if(receivedPackets[i] == rcvBase){
              file  << receivedBuffer[i].payload << std::flush;
              if(receivedBuffer[i].isLast == 1)
                return 0;
              receivedPackets.erase(receivedPackets.begin()+i);
              receivedBuffer.erase(receivedBuffer.begin()+i);
            }
          }
          rcvBase++;
        }while(count(receivedPackets.begin(),receivedPackets.end(),rcvBase));
      }
    }
    else if(seqNo < rcvBase && seqNo >= rcvBase - WIN_SIZE){
      printf("Duplicate Packet Received\n" );
      toSend.type = 2;
      toSend.recvSequenceNumber = seqNo;
      toSend.checksum = getChecksum(received.payload);
      sendto(udpSocket,(Ack *)&toSend,12,0,(struct sockaddr *)&serverStorage,addr_size);
    }
    else {
      printf("in else\n" );
    }
  }
  // file.close();
  printf("File Successfully Received\n" );
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
