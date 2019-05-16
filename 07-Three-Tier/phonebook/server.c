#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "protocol.h"

int main(int argc, char *argv[])
{

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    struct sockaddr_in simpleServer;

    if (2 != argc)
    {

        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (simpleSocket == -1)
    {

        perror("Could not create a socket!");
        exit(1);
    }
    else
    {
        fprintf(stderr, "Socket created!\n");
    }

    /* retrieve the port number for listening */
    simplePort = atoi(argv[1]);

    /* setup the address structure */
    /* use INADDR_ANY to bind to all local addresses  */
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
    simpleServer.sin_port = htons(simplePort);

    /*  bind to the address and port with our socket  */
    returnStatus = bind(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));

    if (returnStatus == 0)
    {
        fprintf(stderr, "Bind completed!\n");
    }
    else
    {
        perror("Could not bind to address!");
        close(simpleSocket);
        exit(1);
    }

    /* lets listen on the socket for connections      */
    returnStatus = listen(simpleSocket, 5);

    if (returnStatus == -1)
    {
        perror("Cannot listen on socket!");
        close(simpleSocket);
        exit(1);
    }

    while (1)
    {

        struct sockaddr_in clientName = {0};
        int simpleChildSocket = 0;
        unsigned int addrlen = sizeof(clientName);

        /* wait here */

        simpleChildSocket = accept(simpleSocket, (struct sockaddr *)&clientName, &addrlen);

        if (simpleChildSocket == -1)
        {

            perror("Cannot accept connections!");
            close(simpleSocket);
            exit(1);
        }

        char buffer[256];

        memset(&buffer, '\0', sizeof(buffer));
        returnStatus = read(simpleChildSocket, buffer, sizeof(buffer));
        if (returnStatus == -1)
        {
            perror("Error reading on buffer");
        }

        char input[256];

        sscanf(buffer, "%s", input);

        printf("%s - \n", input);

        //STARTING_PROTOCOL

        //SEND BACK ACL
        p__send_ack(simpleChildSocket, simpleSocket);

        //WAITING FOR GO
        p__wait_go(simpleChildSocket, simpleSocket);

        //QUERY DB

        FILE *fd;
        if ((fd = fopen("contacts.txt", "r")) == NULL)
        {
            perror("errore apertura file");
            exit(1);
        }
        char name[50];
        char surname[50];
        char number[50];

        while (fscanf(fd, "%s %s %s", name, surname, number) != EOF)
        {

            if (strcmp(input, surname) == 0)
            {
                char buf[256];
                memset(buf, '\0', sizeof(buf));
                sprintf(buf, "%s %s %s", name, surname, number);
                size_t e = write(simpleChildSocket, &buf, sizeof(buf));
                if (e < 0)
                {
                    close(simpleChildSocket);
                    close(simpleSocket);
                    perror("Error writing on buffer");
                    exit(1);
                }
            }
        }

        char buf[10] = "FIN";
        size_t e = write(simpleChildSocket, &buf, sizeof(buf));
        if (e < 0)
        {
            close(simpleChildSocket);
            close(simpleSocket);
            perror("Error writing on buffer");
            exit(1);
        }

        returnStatus = close(simpleChildSocket);
        if (returnStatus == -1)
        {
            perror("Error closing connection");
        }
    }

    close(simpleSocket);
    return 0;
}
