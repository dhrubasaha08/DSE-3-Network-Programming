#include <stdio.h>
#include <assert.h>
#include "unp.h"

typedef enum
{
  SHUTDOWN_NONE = 0,
  SHUTDOWN_RD,
  SHUTDOWN_WR,
  SHUTDOWN_BOTH
}shutdown_enum;

typedef struct
{
  char str[100];
  unsigned int len;
} mystr;

typedef struct {
  mystr store[100];
  int top;
} mystack_t;

typedef struct {
  int sd;
  struct sockaddr_in cid;
  mystack_t *sp;
} cli_ctxt_t;

int num_conn = 0;

void proc_serv_data(cli_ctxt_t *cli)
{
  ssize_t n;
  char recv_buff[MAXBUFFSZ], send_buff[MAXBUFFSZ];
  char fname[50], *loc;
  char *p;
  int sockfd, top, send_buff_off;
  shutdown_enum stype = SHUTDOWN_NONE;

  sockfd = cli->sd;
  while(1)
  {
    n = recv(sockfd, recv_buff, MAXBUFFSZ,0);
    //fputs(recv_buff, stdout);
    if(n > 0)
    {
      bzero(send_buff, MAXBUFFSZ); 
      if(!strncmp(recv_buff, "PUSH", 4)) 
      {
        printf("command : PUSH\n");
        cli->sp->top ++; 
        top = cli->sp->top;
        if(top == 100)
        {
          sprintf(send_buff, "Your stack is full!! Push impossible.");
        }
        else
        {
          assert((n-5) < 100);
          strncpy(cli->sp->store[top].str, &recv_buff[5], n-5);
          cli->sp->store[top].len = n-5;
          sprintf(send_buff, "Pushed %s successfully. Top = %d\n", 
                  cli->sp->store[top].str, top); 
        }
      } 
      else if(!strncmp(recv_buff, "POP", 3))
      {
        printf("command : POP\n");
        top = cli->sp->top;
        if (top == -1)
          sprintf(send_buff, "Stack is empty !! Pop impossible.");
        else
        {
          sprintf(send_buff, "Element %s Popped\n", cli->sp->store[top].str);
          bzero(&cli->sp->store[top], sizeof(mystr));
          cli->sp->top --;
        }
      }
      else if(!strncmp(recv_buff, "DISPLAY", 7))
      {
        printf("command : DISPLAY\n");
        top = cli->sp->top;
        if(top == -1)
          sprintf(send_buff, "Stack is empty !! Nothing to print.");
        else
        {
          send_buff_off = sprintf(send_buff,"Your stack is :\n");
          while(top >= 0)
          {
            assert((send_buff_off+strlen(cli->sp->store[top].str)) < MAXBUFFSZ);
            send_buff_off+=sprintf(&send_buff[send_buff_off], "%s\n", 
                           cli->sp->store[top].str);
            top --;
	  }
        }
      }
      else if(!strncmp(recv_buff, "READ CLOSE", 10))
      {
        printf("command : READ close\n");
        sprintf(send_buff, "You requested Read shutdown \n");
        stype = SHUTDOWN_RD;
      }
      else if(!strncmp(recv_buff, "WRITE CLOSE", 11))
      {
        printf("command : WRITE close\n");
        sprintf(send_buff, "You requested Write shutdown \n");
        stype = SHUTDOWN_WR;
      }
      else if(!strncmp(recv_buff, "READ WRITE CLOSE", 16))
      {
        sprintf(send_buff, "You requested Read Write shutdown \n");
        stype = SHUTDOWN_BOTH;
      }
      if(strlen(send_buff) > 0) 
      {
        if(send(sockfd, send_buff, strlen(send_buff), MSG_NOSIGNAL) < 0)
        {
          if(errno == EPIPE)
          {
            printf("Cannot transmit as this end is shutdown..\n");
            printf("Locally printing : %s\n", send_buff);
          }
          else
            printf("Send error ..\n");
        }
        if(stype == SHUTDOWN_RD)
          shutdown(sockfd, SHUT_RD);
        else if(stype == SHUTDOWN_WR)
          shutdown(sockfd, SHUT_WR);
        else if(stype == SHUTDOWN_BOTH)
        {
          free(cli->sp);
          shutdown(sockfd, SHUT_RDWR);
        }
      }
    }
    else if(n < 0)
    {
      perror("Recv error ..\n");
      continue;
    }
    else if(n == 0)
    {
      if(stype == SHUTDOWN_NONE)
      {
        printf("Client closes connection\n");
        free(cli->sp);
        exit(0);
      }
    }
  }
}


void child_proc_term_hdlr(int signo)
{
  pid_t pid;
  int stat;

  pid = waitpid(pid, &stat, WNOHANG);
  printf("Child %d terminated\n", pid);
  num_conn --;
  if(num_conn == 0)
  {
    printf("All sockets disconnected..\n");
    exit(0); 
  }
}


int main(int argc, char **argv)
{
  int listenfd, connfd, to_exit = 0;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  cli_ctxt_t cli_ctxt[10]; /* Maximum number of connected clients = 10 */
  void send_serv_data(int);
  void child_proc_term_hdlr(int);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  if(bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
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

  if(listen(listenfd, LISTENQ) < 0)
  {
    printf("Listen error ...\n");
    exit(0);
  }

  signal (SIGCHLD, child_proc_term_hdlr);
  for(;;) 
  {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
    memset(&cli_ctxt[num_conn], 0, sizeof(cli_ctxt_t));
    cli_ctxt[num_conn].sd = connfd;

    cli_ctxt[num_conn].sp = (mystack_t *)malloc(sizeof(mystack_t));
    bzero(cli_ctxt[num_conn].sp, sizeof(mystack_t));
    cli_ctxt[num_conn].sp->top = -1;

    cli_ctxt[num_conn].cid.sin_addr = cliaddr.sin_addr;
    cli_ctxt[num_conn].cid.sin_port = cliaddr.sin_port;
    
    if((childpid = fork()) == 0)
    {
      printf("listenfd = %d, connfd =  %d\n", listenfd, connfd);
      close(listenfd);
      proc_serv_data(&cli_ctxt[num_conn]);
      exit(0);
    }
    else
    {
      printf("Client IP address and port number is :\n");
      printf("%s:", inet_ntoa(cliaddr.sin_addr));
      printf("%d\n", ntohs(cliaddr.sin_port));
      printf("Server process id for the client = %d\n", childpid);
      num_conn ++;
    }
  }
}


