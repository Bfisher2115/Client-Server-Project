#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int netserverinit(char* hostname);
int netopen(const char* pathname,int flags);
ssize_t netread(int fildes,void*buf,size_t nbtyes);
ssize_t netwrite(int fildes,const void*buf,size_t nbytes);
int netclose(int fd);
/*netopen flags*/
#define O_WRONLY 0
#define O_RDONLY 1
#define O_RDWR 2
#define PORT 63666

#ifndef CLIENT_H
#define CLIENT_H



#endif