#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERV_PORT 9877
#define MAXBUFFSZ 320
#define LISTENQ 1024
#define MAX_CLIENTS 256
#define MAX_VALUES 5
#define SA struct sockaddr

typedef struct {
    unsigned char id;
    unsigned int numElmt;
    double val[25];
} myMsg_t;

typedef struct {
    unsigned char id;
    double values[MAX_VALUES];
    int value_count;
    double sum;
} client_data_t;

client_data_t clients[MAX_CLIENTS];

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].id = i;
        clients[i].value_count = 0;
        clients[i].sum = 0.0;
        for (int j = 0; j < MAX_VALUES; j++) {
            clients[i].values[j] = 0.0;
        }
    }
}

void update_client_data(unsigned char id, double value, double threshold) {
    client_data_t *client = &clients[id];
    client->sum -= client->values[client->value_count % MAX_VALUES];
    client->values[client->value_count % MAX_VALUES] = value;
    client->sum += value;
    client->value_count++;

    double average = client->sum / (client->value_count < MAX_VALUES ? client->value_count : MAX_VALUES);

    if (average > threshold) {
        printf("Archiving: Client ID %u - Average value: %f\n", id, average);
        // Archive data as needed
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./server_modified <port> <v>\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    double v = atof(argv[2]);

    int sockfd;
    myMsg_t rxMsg;
    char txMsg[MAXBUFFSZ];
    struct sockaddr_in servAddr, cliAddr;
    socklen_t cliAddrLen;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    init_clients();

    for (;;) {
        cliAddrLen = sizeof(cliAddr);
        memset(&rxMsg, 0, sizeof(rxMsg));

        int recv_len = recvfrom(sockfd, &rxMsg, sizeof(rxMsg), 0, (SA *)&cliAddr, &cliAddrLen);
        if (recv_len < 0) {
            perror("recvfrom error");
            exit(1);
        }

        // Send value v to the client
        snprintf(txMsg, sizeof(txMsg), "%.16lf", v);
        if (sendto(sockfd, txMsg, strlen(txMsg), 0, (SA *)&cliAddr, cliAddrLen) < 0) {
            perror("sendto error");
            exit(1);
        }

        // Update client data and check the average
        for (unsigned int i = 0; i < rxMsg.numElmt; i++) {
            update_client_data(rxMsg.id, rxMsg.val[i], 0.75 * v);
        }
    }

    return 0;
}
