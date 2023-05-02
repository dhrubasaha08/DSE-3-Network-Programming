#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include "unp.h"
#include "common.h"


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
        tmp = htond(msg->a[i]);
        memcpy(((unsigned char*)msg->a) + (i * sizeof(double)), &tmp, sizeof(double));
    }
}


int main(int argc, char **argv)
{
    MyMsg_t txMsg;
    unsigned char txbuff[sizeof(MyMsg_t)];
    char rxMsg[10];
    int i, j, select_no, servAddrLen;
    fd_set readfd;
    double v;

    if (argc < 3) {
        printf("Usage : <myclient> <server IP address> <server port>\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servAddr.sin_addr);
    printf("%x\n", ntohl(servAddr.sin_addr.s_addr));
    servAddr.sin_port = htons(atoi(argv[2]));

    // Get the range of values to send from the server
    recvfrom(sockfd, &v, sizeof(double), 0, (struct sockaddr *)&servAddr, &servAddrLen);
    printf("Range of values to send: %f to %f\n", -v, v);

    srand48(17003);

    // Send a message to the server every second, 10 times
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 5; j++) {
            txMsg.a[j] = -v + 2.0 * v * drand48();
            printf("%f ", txMsg.a[j]);
        }
        printf("\n");

        ConvertToNbw(&txMsg, 5);
        memcpy(txbuff, &txMsg, sizeof(MyMsg_t));

        sendto(sockfd, txbuff, sizeof(MyMsg_t), 0, (struct sockaddr *)&servAddr, sizeof(servAddr));

        bzero(rxMsg, 10);
        recvfrom(sockfd, &rxMsg, 10, 0, NULL, NULL);
        fputs(rxMsg, stdout);
        if (!strncmp(rxMsg, "NEXT MSG", 8)) {
            printf("Server->Client: ");
            fputs(rxMsg, stdout);
            printf("\n");
        } else {
            printf("Terminated from server\n");
            break;
        }

        sleep(1);
    }
}
