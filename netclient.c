#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libnetfiles.h"


int main(int argc, char const *argv[])
{
	/*TODO: Do we need to pull parameters from the command line?*/
	/*REMOVE: testing with hard coded ip*/
	char*ip = "basic.cs.rutgers.edu";
    int n;
    int x;
    char buf[256];
	x = netserverinit(ip);
	n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_RDWR);
    printf("FD returned == %d\n",n);
    x = netread(n,buf,100);
    printf("read this many bytes: %d\n",x);
    printf("read: %s\n",buf);
    bzero(buf,256);
    strcat(buf,"i have overwritten this file muahahhahahahahha!!!!");
    x = netwrite(n,buf,strlen(buf));
    printf("write this many bytes: %d\n",x);
    x = netclose(n);
    printf("return on close = %d\n",x);
	return 0;
}