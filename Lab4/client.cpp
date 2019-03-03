 /************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <chrono>

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
  int clientSocket, portNum, nBytes;
  char buffer[LEN];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  char filename[1024];
  printf("Enter the filename to be transferred : " );
  gets(filename);
  ifstream input (filename, ios::binary | ios::in);
  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 50000;
	if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		cout << "Failed to set socket timeout option" << endl;
		return 0;
	}

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*Initialize size variable to be used later on*/

  addr_size = sizeof serverAddr;

  Packet sent,received ;
  int seqNo = 1;
  int AckNo = 1;
  int Ack = 1;
  int checksum = 1;
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  while(!input.eof()){
    //printf("Type a sentence to send to server:\n");
    //fgets(buffer,LEN,stdin);
    //printf("You typed: %s",buffer);

    input.read(buffer,LEN);
    // if(input.eof())
      // break;
    nBytes = strlen(buffer) + 17;
  //  cout << strlen(buffer) << endl;

    sent.header.sequenceNo = seqNo;
    sent.header.Ack = Ack;
    sent.header.AckNo = AckNo;
    sent.header.checksum = getChecksum(buffer);
    strcpy(sent.data,buffer);

      do {
      /*Send message to server*/
      sendto(clientSocket,(Packet *)&sent,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

      /*Receive message from server*/
      nBytes = recvfrom(clientSocket,(Packet *)&received,LEN+16,0,NULL, NULL);
      if(nBytes < 0){
        printf("Timeout!! Resending the Packet\n" );
      }
      // usleep(1000000);
    } while(received.header.Ack != 1 || nBytes < 0) ;

    printf("Received ACK %d for Packet %d . Received ACK NO : %d \n",received.header.Ack,sent.header.sequenceNo,received.header.AckNo);
    seqNo = received.header.AckNo;
    memset(buffer,0, LEN);
  }
  input.close();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time taken to transfer the file = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

  close(clientSocket);
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
