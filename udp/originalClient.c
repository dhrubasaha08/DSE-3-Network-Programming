#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

typedef struct
{
  unsigned int numElmt;
  union mydata{
    float dElmt;
    unsigned int iElmt;
  } a[10];
} MyMsg_t;



int sockfd, to_exit = 0, i, select_no;
socklen_t servAddrLen;
struct sockaddr_in cliAddr, servAddr;


void ConvertToNbw(MyMsg_t *msg, int num)
{
  int i,j;
  unsigned int tmp;
  msg->numElmt = htonl(num);
  for(i=0; i<num; i++)
  {
    tmp = htonl(msg->a[i].iElmt);
    msg->a[i].iElmt = tmp;
  }
}


int main(int argc, char **argv)
{
  MyMsg_t txMsg;
  unsigned char txbuff[80];
  char rxMsg[10];
  int i, j, select_no, servAddrLen;
  fd_set readfd;

  if(argc < 3) {
    printf("Usage : <myclient> <server IP address> <server port>\n");
    exit(0);
  }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    switch(errno)
    {
      case EACCES:
        printf("Permission to create socket of the specified type is denied\n");
      break; 

      case EAFNOSUPPORT:
        printf("Socket of given address family not supported\n");
      break;

      case EINVAL:
        printf("Invalid values in type\n");
      break;

      default:
        printf("Other socket errors\n");
      break;
    }
    exit(0);
  }

  bzero(&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  inet_pton(AF_INET, argv[1], &servAddr.sin_addr);
  printf("%x\n", ntohl(servAddr.sin_addr.s_addr));
  servAddr.sin_port = htons(atoi(argv[2]));

  srand48(17003);

  for(;;)
  { 
    for(i=0; i<10; i++)
    {
      txMsg.a[i].dElmt = (float)(-10.0+20.0*drand48());
      printf("%e ", txMsg.a[i].dElmt);
    }
    printf("\n");

    ConvertToNbw(&txMsg, 10);  

    sendto(sockfd, &txMsg, sizeof(txMsg),0, 
         (struct sockaddr *)&servAddr, sizeof(servAddr));

    bzero(rxMsg, 10);
    recvfrom(sockfd, &rxMsg, 10, 0, (struct sockaddr *)&servAddr, &servAddrLen);
    fputs(rxMsg, stdout);
    if(!strncmp(rxMsg,"NEXT MSG",10))
    {
      printf("Sever->client : ");
      fputs(rxMsg, stdout);
      printf("\n");
    }
    else
    {
      printf("Terminated from server\n");
      break;
    }
  }
}