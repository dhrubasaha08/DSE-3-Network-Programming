#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAXBUFFSZ 320
#define SA struct sockaddr

typedef struct {
    unsigned char id;
    unsigned int numElmt;
    double val[25];
} myMsg_t;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: ./client_modified <IP address> <port> <id>\n");
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servAddr;
    myMsg_t txMsg;
    char rxMsg[MAXBUFFSZ];
    unsigned char clientId;
    clientId = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    txMsg.id = atoi(argv[3]);
    txMsg.numElmt = 25;
    srand(time(0) * clientId);

    for (unsigned int i = 0; i < txMsg.numElmt; i++) {
        txMsg.val[i] = (double)(rand() % 1000) / 100;
    }

    socklen_t servAddrLen = sizeof(servAddr);
    if (sendto(sockfd, &txMsg, sizeof(txMsg), 0, (SA *)&servAddr, servAddrLen) < 0) {
        perror("sendto error");
        exit(1);
    }

    // Receive value v from the server
    if (recvfrom(sockfd, rxMsg, sizeof(rxMsg), 0, (SA *)&servAddr, &servAddrLen) < 0) {
        perror("recvfrom error");
        exit(1);
    }

    double v;
    sscanf(rxMsg, "%lf", &v);
    printf("Received value v: %f\n", v);

    return 0;
}
