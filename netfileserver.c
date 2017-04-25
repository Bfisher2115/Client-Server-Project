#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>



int main(int argc, char const *argv[])
{
	int sockfd, inSockfd;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
	/*Port no: hard coded to 63666*/
    int portno = 63666;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
    	puts("Error occured when opening the socket.");
    	return 0;
    }
    /*set the feilds of the Sockaddr_in struct*/
    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    /*Htons: special function for converting to */
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    /*Names the socket if bind succeeds with newly created socket struct*/
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    	error("Error on binding.");
    /*listen stores incoming calls to socket in a queue*/
    listen(sockfd);
    /*accept takes top item off of queue in listen and creates a new connected socket
    and returns a FD*/
    while(inSockfd = accept(sockfd,(struct sockaddr*)&cli_addr,sizeof(cli_addr))){
    	if (inSockfd < 0) 
          error("ERROR on accept");
     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);
     	if (n < 0) 
     		error("ERROR reading from socket");
	    printf("Here is the message: %s\n",buffer);
	    n = write(newsockfd,"I got your message",18);
	    if (n < 0) 
	    	error("ERROR writing to socket");
	    break;
    }
	return 0;
}