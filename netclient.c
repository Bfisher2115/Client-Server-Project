#include <stdlib.h>
#include <stdio.h>



int main(int argc, char const *argv[])
{
	/*TODO: Do we need to pull parameters from the command line?*/
	/*REMOVE: testing with hard coded ip*/
	char*ip = "basic.cs.rutgers.edu";
    int n;
	n = netserverinit(ip);
	if(n == -1){
		puts("Error retireving hostname");
		return 0;
	}
	n = netopen("/.autofs/ilab/ilab_users/bgf23/Client-Server-Project/testfile.txt",O_WRONLY);
    /*printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
    	puts("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    	puts("ERROR reading from socket");
    printf("%s\n",buffer);*/
	return 0;
}