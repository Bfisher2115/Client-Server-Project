#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>

int netserverinit(char* hostname,int filemode);
int netopen(const char* pathname,int flags);
ssize_t netread(int fildes,void*buf,size_t nbtyes);
ssize_t netwrite(int fildes,const void*buf,size_t nbytes);
int netclose(int fd);
/*netopen flags*/
#define PORT 63667
// define macro for filemode
#define UNRESTRICTED 0
#define EXCLUSIVE 1
#define TRANSACTION 2
// for client if undef
#ifndef O_RDONLY
#define O_RDONLY	0
#define O_WRONLY	1
#define O_RDWR		2
#endif
// 
#ifndef EBADF

#define EBADF            9
#define EPERM            1
#define EINTR            4
#define ENOENT           2
#define EISDIR          21
#define EACCES          13
#define EROFS           30
#define ENFILE          23
#define EWOULDBLOCK     EAGAIN
#define ETIMEDOUT       110 
#define ECONNRESET      104




#endif