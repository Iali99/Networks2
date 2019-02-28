/************* SENDER CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <chrono>

using namespace std;

#define LEN 500
#define WIN_SIZE 2
//Header Structure
struct DataPack {
    int type;
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

// Thread Receiver Function
void* receiverThread(void * args);

// Sender Thread Funtion
void* senderThread(void * args);

int getChecksum(char * a);

/* ---- Global Variables ---- */

vector<int> receivedPackets;
int base, nextSeqNumber, clientSocket;
int totalPackets;
struct sockaddr_in serverAddr;
socklen_t addr_size;
vector<DataPack> packets;


/* -- Main Program -- */

int main(){
  int clientSocket, portNum, nBytes;
  char buffer[LEN];


  char filename[1024];
  printf("Enter the filename to be transferred : " );
  gets(filename);
  ifstream input (filename, ios::binary | ios::in );
  int filesize = input.tellg();

  DataPack tempPacket;
  totalPackets = 0;
  while(!input.eof()) {
    input.read(buffer,LEN);
    strcpy(tempPacket.payload, buffer);
    tempPacket.payloadLength = strlen(buffer);
    tempPacket.sequenceNo = totalPackets;
    totalPackets++;
    tempPacket.type = 1; // 1 for DataPack
    tempPacket.checksum = getChecksum(buffer);
    memset(buffer,0,LEN);
    packets.push_back(tempPacket);
  }
  printf("totalPackets = %d\n",totalPackets );
  base = 0;
  nextSeqNumber = 0;
//  int totalPackets = (filesize+LEN-1)/LEN;

  for(int i = 0;i<totalPackets;i++)
    receivedPackets.push_back(0);
  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);


  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*Initialize size variable to be used later on*/

  addr_size = sizeof serverAddr;


  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  pthread_t senderT,receiverT;
  pthread_create(&senderT,NULL,senderThread,(void *)1);
  pthread_create(&receiverT,NULL,receiverThread,(void *)2);

  pthread_join(senderT,NULL);
  pthread_join(receiverT,NULL);

  input.close();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time taken to transfer the file = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

  close(clientSocket);
  return 0;
}

void * senderThread(void * args) {
  while(1) {
    if(nextSeqNumber == totalPackets)
      break;
    if(nextSeqNumber < base + WIN_SIZE) {
      printf("sending packet with seq no %d\n",nextSeqNumber );
      sendto(clientSocket, (DataPack *)&(packets[nextSeqNumber]),strlen(packets[nextSeqNumber].payload)+16,0,(struct sockaddr *)&serverAddr,addr_size);
      // printf("seq no ++\n");
      nextSeqNumber++;
    }
  }
  printf("Out of while loop\n" );
}

void * receiverThread(void * args) {
  Ack recvPacket;
  while(1) {
    int nBytes = recvfrom(clientSocket,(Ack *)&recvPacket,12,0,NULL,NULL);
    receivedPackets[recvPacket.recvSequenceNumber] = 1;
    printf("received ack for %d packet\n",recvPacket.recvSequenceNumber );
    while(receivedPackets[base]){
      base++;
    }
  }
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
