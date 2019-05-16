#include "protocol.h"

void xclose(int fd)
{
    ssize_t e = close(fd);
    if (e != 0)
    {
        perror("Error closing fd");
        exit(1);
    }
}

void p__send_ack(int socket_c, int socket)
{
    char ack[10] = ACK;
    memset(&ack, '\0', sizeof(ack));
    size_t e = write(socket_c, &ack, sizeof(ack));
    if (e < 0)
    {
        xclose(socket_c);
        xclose(socket);
        perror("Error writing on buffer");
        exit(1);
    }
}

void p__wait_go(int socket_c, int socket)
{
    char resp[10];
    memset(&resp, '\0', sizeof(resp));
    ssize_t e = read(socket_c, &resp, sizeof(resp));
    if (e < 0)
    {
        xclose(socket_c);
        xclose(socket);
        perror("Error reading on buffer");
        exit(1);
    }
    if (strcmp(resp, GO) != 0)
    {
        xclose(socket_c);
        xclose(socket);
        puts("Protocol Error: NOT GO RECEIVED");
        exit(1);
    }else{
        puts("GO Received\n");
    }
}