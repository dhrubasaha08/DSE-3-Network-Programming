#include "unp.h"

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr;
  void str_cli (FILE *, int);

  if(argc < 2)
    perror("Usage : myclient <IPADDR>");

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  if(argc == 3)
    servaddr.sin_port = htons(atoi(argv[2]));
  else
    servaddr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  if(connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
  {
    switch(errno)
    {
      case ETIMEDOUT : 
        printf("Timed out for TCP SYN ..\n");
        break;

      case EHOSTUNREACH : 
        printf("Host unreachable error ...\n");
        break;

      case ENETUNREACH : 
        printf("Network unreachable error ...\n");
        break;

      case ECONNREFUSED : 
        printf("Connection refused on requested port ...\n");
        break;

      default :
        printf("Unknown connect error ...\n");
        break;
    }
    exit(0);
  } 

  str_cli(stdin, sockfd);

  exit(0);

}


void str_cli (FILE *fp, int sockfd)
{
  char sendline[MAXBUFFSZ], recvline[MAXBUFFSZ];
  socklen_t clilen, servlen;
  struct sockaddr_in cliaddr, servaddr;
  char serv_ip_addr[16];

  bzero(sendline, MAXBUFFSZ);
  bzero(recvline, MAXBUFFSZ);

  getpeername(sockfd, (SA *)&servaddr, &servlen);
  inet_ntop(AF_INET, &servaddr.sin_addr, serv_ip_addr, 16);

  printf("Server IP address and port number is :\n");
  printf("%s:", serv_ip_addr);
  printf("%d\n", ntohs(servaddr.sin_port));
  
  while(fgets(sendline, MAXBUFFSZ, fp)!= NULL)
  {
    if(!strncmp(sendline, "Close", 5))
    {
      printf("command = close\n");
      close(sockfd);
      exit(0);
    }
    
    printf("----- Sending to server :  ------\n");
    fputs(sendline, stdout);
    printf("---------------------------------\n\n");
    send(sockfd, sendline, strlen(sendline),0);
    if(!strncmp(sendline, "READ CLOSE",10))
    {
      shutdown(sockfd,SHUT_WR);
    }
    if(recv(sockfd, recvline, MAXBUFFSZ,0) == 0)
    {
      printf("Server sending zero-sized message\n");
    }

    printf("==================================================\n");
    fputs(recvline, stdout);
    printf("==================================================\n");
    bzero(sendline, MAXBUFFSZ);
    bzero(recvline, MAXBUFFSZ);
  }
}

