#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "libnetfiles.h"

int netserverinit(char* hostname){
	struct hostent* temp = gethostbyname(hostname);
	if(temp == NULL){
		return -1;
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	/*REMOVE: testing with hard coded ip*/
	char*ip = "basic.cs.rutgers.edu";
	/*Port no: hard coded to 63666*/
	portno = 63666;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
		puts("Error opening socket.");
	/*Zero out mem*/
	memset(buffer,'0',sizeof(buffer));
	memset(&serv_addr,'0',sizeof(serv_addr));
	n = netserverinit(ip);
	if(n == -1){
		puts("Error retireving hostname");
		return -1;
	}
	server = gethostbyname(ip);
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
 		puts("ERROR connecting");
    }
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
    	puts("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    	puts("ERROR reading from socket");
    printf("%s\n",buffer);
	return 0;
}