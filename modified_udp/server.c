#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct
{
    unsigned char id;
    unsigned int numElmt;
    double val[25];
} myMsg_t;

typedef struct
{
    unsigned char id;
    double v;
} vMsg_t;

typedef struct
{
    struct sockaddr_in addr;
    double values[5];
    int numValues;
    double total;
} clientInfo_t;

clientInfo_t clients[256];

void add_value(int index, double value)
{
    if (clients[index].numValues < 5)
    {
        clients[index].values[clients[index].numValues++] = value;
        clients[index].total += value;
    }
    else
    {
        clients[index].total -= clients[index].values[0];
        memmove(clients[index].values, clients[index].values + 1, sizeof(double) * 4);
        clients[index].values[4] = value;
        clients[index].total += value;
    }
}

void handle_message(myMsg_t *msg, struct sockaddr_in *addr, int sockfd, double v)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

    unsigned int port = ntohs(addr->sin_port);
    char client_id[INET_ADDRSTRLEN + 8]; // IP + port
    sprintf(client_id, "%s %d %d", ip, port, msg->id);

    FILE *log_file = fopen("log.txt", "a");

    for (int i = 0; i < msg->numElmt; ++i)
    {
        double value = msg->val[i];
        add_value(msg->id, value);

        double average = (clients[msg->id].numValues) ? (clients[msg->id].total / clients[msg->id].numValues) : 0;
        if (average > 0.75 * v)
        {
            fprintf(log_file, "Client %s has average %f exceeding 0.75*v\n", client_id, average);
        }
    }

    fclose(log_file);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <server port> <v>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);
    double v = atof(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("Could not create socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Could not bind socket");
        return 1;
    }

    memset(clients, 0, sizeof(clients));

    char buffer[1024];
    while (1)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int msgLen = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                              (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (msgLen < 0)
        {
            perror("Failed to receive data");
        }
        else if (msgLen == sizeof(myMsg_t))
        {
            handle_message((myMsg_t *)buffer, &clientAddr, sockfd, v);
        }
        else if (msgLen == sizeof(vMsg_t))
        {
            vMsg_t *msg = (vMsg_t *)buffer;
            if (msg->id == 0)
            {
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(clientAddr.sin_addr), client_ip, INET_ADDRSTRLEN);
                printf("Received connection request from client %s %d %d\n",
                       client_ip, ntohs(clientAddr.sin_port), msg->id);
                vMsg_t response = {0, v};
                if (sendto(sockfd, &response, sizeof(response), 0,
                           (struct sockaddr *)&clientAddr, clientAddrLen) == -1)
                {
                    perror("Failed to send v");
                }
            }
        }
    }

    close(sockfd);
    return 0;
}