#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libnetfiles.h"



int main(int argc, char const *argv[])
{
	/*TODO: Do we need to pull parameters from the command line?*/
	/*REMOVE: testing with hard coded ip*/
	char*ip = "basic.cs.rutgers.edu";
    //const char*test_w = "hello there";
    int n;
    //int x;
    // schar buf[256];

	netserverinit(ip,TRANSACTION);
	n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_WRONLY);
    printf("FD returned == %d\n",n);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_RDONLY);
    printf("FD returned == %d\n",n);
    
    netserverinit(ip,UNRESTRICTED);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_WRONLY);
    printf("FD returned == %d\n",n);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_RDONLY);
    printf("FD returned == %d\n",n);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_RDWR);
    printf("FD returned == %d\n",n);

    netserverinit(ip,TRANSACTION);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_RDONLY);
    printf("FD returned == %d\n",n);
    n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_WRONLY);
    printf("FD returned == %d\n",n);

    // x = netread(n,buf,42);
    // printf("read this many bytes: %d\n",x);
    // printf("read: %s\n",buf);

    // bzero(buf,256);
    // x = netwrite(n,test_w,strlen(test_w));
    // printf("write this many bytes: %d\n",x);

    // bzero(buf,256);
    // x = netread(n,buf,x);
    // printf("read this many bytes: %d\n",x);
    // printf("read: %s\n",buf);

    // bzero(buf,256);
    // x = netwrite(n,test_w,strlen(test_w));
    // printf("write this many bytes: %d\n",x);



    // x = netclose(n);
    // printf("return on close = %d\n",x);

	return 0;
}