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

#define LEN 100
#define WIN_SIZE 4
#define TIME_OUT 20000
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

// Thread Receiver Function
void* receiverThread(void * args);

// Sender Thread Funtion
void* senderThread(void * args);

void * timeoutCheckerThread(void * args);

int getChecksum(char * a);

/* ---- Global Variables ---- */

int c;
vector<int> receivedPackets;
int base, nextSeqNumber, clientSocket;
int totalPackets;
struct sockaddr_in serverAddr;
socklen_t addr_size;
vector<DataPack> packets;
vector<std::chrono::system_clock::time_point> packetSendTimes;

/* -- Main Program -- */

int main(){
  int  portNum, nBytes;
  char buffer[LEN];
  c = 0;

  char filename[1024];
  printf("Enter the filename to be transferred : " );
  gets(filename);
  ifstream input (filename, ios::binary | ios::in );
  int filesize = input.tellg();

  DataPack tempPacket;
  totalPackets = 0;
  while(!input.eof()) {
    memset(buffer,0,LEN);
    input.read(buffer,LEN);
    strcpy(tempPacket.payload, buffer);
    tempPacket.payloadLength = strlen(buffer);
    tempPacket.sequenceNo = totalPackets;
    totalPackets++;
    tempPacket.type = 1; // 1 for DataPack
    tempPacket.checksum = getChecksum(buffer);
    // cout << buffer << "\n ------------------------------\n";

    packets.push_back(tempPacket);
  }
  packets[totalPackets - 1].isLast = 1;
  cout << endl;
  printf("totalPackets = %d\n",totalPackets );
  base = 0;
  nextSeqNumber = 0;
//  int totalPackets = (filesize+LEN-1)/LEN;

  for(int i = 0;i<totalPackets;i++){
    receivedPackets.push_back(0);
    packetSendTimes.push_back(std::chrono::system_clock::now());
  }
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

  pthread_t senderT,receiverT,timeoutT;
  pthread_create(&senderT,NULL,senderThread,(void *)1);
  pthread_create(&receiverT,NULL,receiverThread,(void *)2);
  pthread_create(&timeoutT,NULL,timeoutCheckerThread,(void *)3);
  pthread_join(senderT,NULL);
  pthread_join(receiverT,NULL);
  pthread_join(timeoutT,NULL);
  input.close();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time taken to transfer the file = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

  close(clientSocket);
  return 0;
}

void * timeoutCheckerThread(void * args) {
  std::chrono::microseconds timeout (TIME_OUT);
  while(1) {
    if(nextSeqNumber == totalPackets)
      break;
    for(int i = base;i<base+WIN_SIZE;i++){
      if(i< totalPackets && receivedPackets[i] != 1 ) {
        auto now = std::chrono::system_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<chrono::microseconds>(now-packetSendTimes[i]);
        if(duration.count() > timeout.count()){
          printf("Resending packet %d\n", i);
          sendto(clientSocket, (DataPack *)&(packets[i]),strlen(packets[i].payload)+16,0,(struct sockaddr *)&serverAddr,addr_size);
        }
      }
    }
  }
}

void * senderThread(void * args) {

  while(1) {
    if(nextSeqNumber == totalPackets)
      break;
    if(nextSeqNumber < base + WIN_SIZE) {
      printf("sending packet with seq no %d\n",nextSeqNumber );
      if(sendto(clientSocket, (DataPack *)&(packets[nextSeqNumber]),strlen(packets[nextSeqNumber].payload)+16,0,(struct sockaddr *)&serverAddr,addr_size) == -1){
        printf("Error sending\n");
      }
      else {
        packetSendTimes[nextSeqNumber] = std::chrono::system_clock::now();
        nextSeqNumber++;
      }
      // printf("seq no ++\n");

    }
  }
}

void * receiverThread(void * args) {
  Ack recvPacket;
  while(1) {
    if(nextSeqNumber == totalPackets)
      break;
    int nBytes = recvfrom(clientSocket,(Ack *)&recvPacket,12,0,NULL,NULL);
    if(nBytes != -1)
    {receivedPackets[recvPacket.recvSequenceNumber] = 1;
    printf("received ack for %d packet %d\n",recvPacket.recvSequenceNumber,c++ );
    while(receivedPackets[base]){
      base++;
    }}
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
