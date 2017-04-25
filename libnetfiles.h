#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#ifndef CLIENT_H
#define CLIENT_H
int sockfd,n;
struct sockaddr_in serv_addr;
struct hostent* server;
int portno;
char buffer[256];

#endif