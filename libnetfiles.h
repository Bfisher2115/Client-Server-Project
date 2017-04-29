#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>

int netserverinit(char* hostname);
int netopen(const char* pathname,int flags);
ssize_t netread(int fildes,void*buf,size_t nbtyes);
ssize_t netwrite(int fildes,const void*buf,size_t nbytes);
int netclose(int fd);
/*netopen flags*/
#define PORT 63667

#ifndef CLIENT_H
#define CLIENT_H




#endif