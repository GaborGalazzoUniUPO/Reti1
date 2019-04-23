#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    char buffer[256] = "";
    struct sockaddr_in simpleServer;

    int x;

    if (4 != argc)
    {

        fprintf(stderr, "Usage: %s <server> <port> <x>\n", argv[0]);
        exit(1);
    }

    x = atoi(argv[3]);

    /* create a streaming socket      */
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (simpleSocket == -1)
    {

        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }
    else
    {
        fprintf(stderr, "Socket created!\n");
    }

    /* retrieve the port number for connecting */
    simplePort = atoi(argv[2]);

    /* setup the address structure */
    /* use the IP address sent as an argument for the server address  */
    //bzero(&simpleServer, sizeof(simpleServer));
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    //inet_addr(argv[2], &simpleServer.sin_addr.s_addr);
    simpleServer.sin_addr.s_addr = inet_addr(argv[1]);
    simpleServer.sin_port = htons(simplePort);

    /*  connect to the address and port with our socket  */
    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));

    if (returnStatus == 0)
    {
        fprintf(stderr, "Connect successful!\n");
    }
    else
    {
        fprintf(stderr, "Could not connect to address!\n");
        close(simpleSocket);
        exit(1);
    }

    returnStatus = write(simpleSocket, argv[3], strlen(argv[3]));
    if (returnStatus < 0)
    {
        perror("Invio della quantita fallito");
        close(simpleSocket);
        exit(1);
    }

    printf("Attendo %d messaggi...\n", x);

    for (int i = 0; i < x; i++)
    {
        printf("Messaggio %d > ", i);
        returnStatus = scanf("%s", buffer);
        if (returnStatus != 1)
        {
            perror("Errore lettura stdin");
            close(simpleSocket);
            exit(1);
        }

        returnStatus = write(simpleSocket, buffer, strlen(buffer));
        if (returnStatus > 0)
        {
            printf("%d: invio del messaggio %d\n", returnStatus, i);
        }
        else
        {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
            close(simpleSocket);
            exit(1);
        }

        returnStatus = read(simpleSocket, buffer, sizeof(buffer));

        if (returnStatus > 0)
        {
            printf("%d: risposta %d - %s\n", returnStatus, i, buffer);
        }
        else
        {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
            close(simpleSocket);
            exit(1);
        }
    }

    returnStatus = write(simpleSocket, "BYE", 3);
    if (returnStatus < 0)
    {
        fprintf(stderr, "Return Status = %d \n", returnStatus);
        close(simpleSocket);
        exit(1);
    }

    while (1)
    {
        returnStatus = read(simpleSocket, buffer, sizeof(buffer));
        if (returnStatus > 0)
        {
            if (strcmp(buffer, "OK") == 0)
                break;
        }
        else
        {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
            close(simpleSocket);
            exit(1);
        }
    }

    close(simpleSocket);

    return 0;
}
