#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int main(int argc, char *argv[])
{

    int simplePort = 0;
    int returnStatus = 0;
    struct sockaddr_in simpleServer;

    if (2 != argc)
    {

        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenfd < 0)
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
    returnStatus = bind(listenfd, (struct sockaddr *)&simpleServer, sizeof(simpleServer));

    if (returnStatus == 0)
    {
        fprintf(stderr, "Bind completed!\n");
    }
    else
    {
        fprintf(stderr, "Could not bind to address!\n");
        close(listenfd);
        exit(1);
    }

    /* lets listen on the socket for connections      */
    returnStatus = listen(listenfd, 5);

    if (returnStatus == -1)
    {
        fprintf(stderr, "Cannot listen on socket!\n");
        close(listenfd);
        exit(1);
    }

    char buffer[256] = "";
    while (1)
    {
        struct sockaddr_in clientName = {0};
        int simpleChildSocket = 0;
        unsigned int addrlen = sizeof(clientName);

        /* wait here */

        simpleChildSocket = accept(listenfd, (struct sockaddr *)&clientName, &addrlen);

        if (simpleChildSocket == -1)
        {

            fprintf(stderr, "Cannot accept connections!\n");
            close(listenfd);
            exit(1);
        }

        pid_t pid = fork();

        if (simpleChildSocket < 0)
        {

            fprintf(stderr, "Cannot fork the process!\n");
            close(listenfd);
            exit(1);
        }

        if (pid == 0)
        {
            close(listenfd);

            puts("soket trovato");

            while (1)
            {
                memset(&buffer, '\0', 256);
                int n;
                if ((n = read(simpleChildSocket, &buffer, 256)) < 0)
                {
                    perror("errore lettura figlio");
                    exit(1);
                }
                else if (n == 0)
                {
                    if (close(simpleChildSocket) < 0)
                    {
                        perror("errore chiusura figlio");
                        exit(0);
                    }
                }
                else
                {
                    if ((n = write(simpleChildSocket, &buffer, strlen(buffer))) < 0)
                    {
                        perror("errore scrittura figlio");
                        exit(1);
                    }
                }
            }
        }
        else
        {
            close(simpleChildSocket);
        }
    }

    close(listenfd);
    return 0;
}
