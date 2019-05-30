#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int main(int argc, char *argv[])
{

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    char buffer[256] = "";
    struct sockaddr_in simpleServer;

    if (3 != argc)
    {

        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
        exit(1);
    }

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

    fd_set rset;

    FD_ZERO(&rset);

    int timeout_c = 0;

    while (1)
    {

        FD_SET(STDIN_FILENO, &rset);
        FD_SET(simpleSocket, &rset);

        struct timeval timeout = {3, 0};

        int max_select = MAX(1, simpleSocket) + 1;

        int sr = 0;
        if ((sr = select(max_select, &rset, NULL, NULL, &timeout)) < 0)
        {
            perror("Errore select");
            exit(1);
        }
        else if (sr == 0)
        {
            switch (timeout_c)
            {
            case 0:
                puts("Are you there?");
                break;
            case 1:
                puts("I'm going to left you out");
                break;

            default:
                puts("I'm quitting you");
                close(simpleSocket);
                exit(0);
                break;
            }
            timeout_c++;
        }
        else
        {
            if (FD_ISSET(STDIN_FILENO, &rset))
            {
                timeout_c = 0;
                memset(&buffer, '\0', 256);
                if (read(STDIN_FILENO, buffer, 256) < 0)
                {
                    perror("Errore read da stdin");
                    exit(1);
                }

                returnStatus = write(simpleSocket, buffer, strlen(buffer));
                if (returnStatus < 0)
                {
                    perror("errore scrittura soket");
                    exit(1);
                }
            }
            else if (FD_ISSET(simpleSocket, &rset))
            {
                memset(&buffer, '\0', 256);
                returnStatus = read(simpleSocket, buffer, sizeof(buffer));
                if (returnStatus < 0)
                {
                    perror("errore lettura soket");
                    exit(1);
                }
                else if (returnStatus == 0)
                {
                    perror("server has closed connection");
                    exit(1);
                }
                else
                    printf("Server returned: %s\n", buffer);
            }
        }
    }

    close(simpleSocket);
    return 0;
}
