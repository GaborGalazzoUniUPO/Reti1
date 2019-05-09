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
#include <stdbool.h>

typedef struct banned_ip {

    in_addr_t addr;
    struct banned_ip *next;

} banned_ip_t;

typedef struct request {

    in_addr_t addr;
    bool warning;
    time_t last_request;
    struct request *next;

} request_t;

bool is_banned(in_addr_t addr, banned_ip_t* banned_list){
    while(banned_list != NULL){
        if(memcmp(&banned_list->addr,&addr,4))
            return true;
    }
    return false;
}

request_t* last_request(in_addr_t addr, request_t* requests){
    while(requests != NULL){
        if(memcmp(&requests->addr,&addr,4))
            return requests;
    }
    return NULL;
}


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

        perror("Could not create a socket!\n");
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

    banned_ip_t* banned_list = NULL;

    request_t* requests = NULL;


    while (1)
    {
        ssize_t e = recvfrom(simpleSocket, buffer, 256, 0, (struct sockaddr *)&senderAddr, &addrlen);
        if(e == -1){
            perror("Error reciving from UDP");
            break;
        }

        time_t rawtime;
       
        if(is_banned(senderAddr.sin_addr.s_addr, banned_list))
            continue;
        
        request_t* last_same_request = last_request(senderAddr.sin_addr.s_addr, requests);

        if(last_same_request == NULL){
            request_t request;
            request.addr = senderAddr.sin_addr.s_addr;
            
            request.last_request = time(&rawtime);
            request.warning = false;
            request.next = requests;
            requests = &request;
        }else{
            time(&rawtime);
            printf("%lf\n", difftime(time(&rawtime),last_same_request->last_request));
            if(difftime(rawtime,last_same_request->last_request)<10){
                if(last_same_request->warning){
                    banned_ip_t banned_ip;
                    banned_ip.addr = last_same_request->addr;
                    banned_ip.next = banned_list;
                    banned_list = &banned_ip;
                    printf("NEW BAN - %s\n", inet_ntoa(senderAddr.sin_addr));
                    continue;
                }else{
                    last_same_request->warning = true;
                     printf("NEW WARNING - %s\n", inet_ntoa(senderAddr.sin_addr));
                     e = sendto(simpleSocket, "please wait", strlen("please wait"), 0, (struct sockaddr *)&senderAddr, addrlen);
                    if(e == -1){
                        perror("Error sending to UDP");
                        break;
                    }
                    continue;
                }
                last_same_request->last_request = time(NULL);
            }
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
