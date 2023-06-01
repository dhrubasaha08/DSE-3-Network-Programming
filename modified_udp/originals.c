#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>


typedef struct
{
  unsigned int numElmt;
  union mydata{
    float dElmt;
    unsigned int iElmt;
  } a[10];
} MyMsg_t;


void ConvertToHostByteOrder(MyMsg_t *msg)
{
  int i,j;
  unsigned char rb[80];

  msg->numElmt = ntohl(msg->numElmt);

  for(i=0; i<(msg->numElmt); i++)
  {
    msg->a[i].iElmt = ntohl(msg->a[i].iElmt); 
  }

  for(i=0; i<msg->numElmt; i++)
    printf("%e ", msg->a[i].dElmt);
  printf("\n");

}


int main(int argc, char *argv[])
{
  MyMsg_t rxMsg;
  char *txMsg = "NEXT MSG";
  int sockfd, numRxMsg;
  unsigned short cliPort;
  struct sockaddr_in servAddr, cliAddr; 
  char cli_ip_addr[16];
  socklen_t cliAddrLen;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  bzero(&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(atoi(argv[1]));
  numRxMsg = atoi(argv[2]);
  if(bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
  {
    if(errno == EADDRINUSE)
    {
      printf("Bind error ... Port is still in use\n");
    }
    else
    {
      printf("Bind error ... \n"); 
    }
    exit(0);
  }
  cliAddrLen = sizeof(cliAddr);
  bzero(cli_ip_addr, 16);

  while(recvfrom(sockfd, &rxMsg, sizeof(rxMsg), 0, (struct sockaddr *)&cliAddr, &cliAddrLen))
  {
    inet_ntop(AF_INET, &(cliAddr.sin_addr), &cli_ip_addr[0], 16); 
    printf("Client IP address : "); 
    puts(cli_ip_addr);
    cliPort = ntohs(cliAddr.sin_port);
    printf("Client port id : %u\n", cliPort);
    ConvertToHostByteOrder(&rxMsg);
    numRxMsg --;
    if(numRxMsg == 0)
    {
      sendto(sockfd, "STOP", 5, 0, (struct sockaddr *)&cliAddr, cliAddrLen);
      break;
    }
    else
    {
      sleep(1);
      printf("Asking for next message\n");
      sendto(sockfd, txMsg, sizeof(txMsg), 0, (struct sockaddr *)&cliAddr, cliAddrLen); 

    }
  }
  printf("%x\n", cliAddr.sin_addr.s_addr);
}