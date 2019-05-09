#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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

    simpleSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (simpleSocket == -1)
    {

        fprintf(stderr, "Could not create a socket!\n");
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
        fprintf(stderr, "Could not bind to address!\n");
        close(simpleSocket);
        exit(1);
    }

    char buffer[256];
    struct sockaddr_in senderAddr;
    socklen_t addrlen; 


    while (1)
    {
        ssize_t e = recvfrom(simpleSocket, buffer, 256, 0, (struct sockaddr *)&senderAddr, &addrlen);
        if(e == -1){
            perror("Error reciving from UDP");
            break;
        }

        printf("Sender IP: %s Post: %d Message: %s\n", inet_ntoa(senderAddr.sin_addr), ntohs(senderAddr.sin_port), buffer);

        e = sendto(simpleSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&senderAddr, addrlen);
        if(e == -1){
            perror("Error sending to UDP");
            break;
        }
    }

    close(simpleSocket);
    return 0;
}
