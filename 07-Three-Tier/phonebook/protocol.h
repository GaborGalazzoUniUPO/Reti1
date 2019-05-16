
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ACK "ACK\n"
#define GO "GO\n"

void p__send_ack(int socket_c, int socket);
void p__wait_go(int socket_c, int socket);

