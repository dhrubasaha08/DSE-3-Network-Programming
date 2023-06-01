#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

typedef struct
{
    unsigned char id;
    unsigned int numElmt;
    double val[25];
} myMsg_t;

typedef struct
{
    unsigned char id; // will be set to 0 to indicate this is a vMsg
    double v;
} vMsg_t;

double generate_random_double(double v)
{
    return ((double)rand() / RAND_MAX) * (2 * v) - v;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <server IP address> <server port>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("Could not create socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    if (inet_aton(server_ip, &serverAddr.sin_addr) == 0)
    {
        perror("Invalid server IP address");
        return 1;
    }

    // Send a connection request to the server
    vMsg_t request = {0, 0};
    if (sendto(sockfd, &request, sizeof(request), 0,
               (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Failed to send connection request");
        return 1;
    }

    // Now, receive the "v" value from the server
    vMsg_t vMsg;
    int msgLen = recvfrom(sockfd, &vMsg, sizeof(vMsg), 0, NULL, NULL);
    if (msgLen < 0)
    {
        perror("Failed to receive data");
        return 1;
    }
    else if (msgLen > 0 && vMsg.id == 0)
    {
        printf("Received v: %f\n", vMsg.v);
    }

    // Initialize random number generator
    srand(time(NULL));

    // Continue sending regular messages with random data
    myMsg_t msg;
    msg.id = 1;      // client ID
    msg.numElmt = 5; // send 5 elements at a time

    while (1)
    {
        for (int i = 0; i < msg.numElmt; ++i)
        {
            msg.val[i] = generate_random_double(vMsg.v);
            printf("Generated value: %f\n", msg.val[i]);
        }

        if (sendto(sockfd, &msg, sizeof(msg), 0,
                   (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        {
            perror("Failed to send data");
        }
        sleep(1); // sleep for a second before sending another packet of data
    }

    close(sockfd);
    return 0;
}