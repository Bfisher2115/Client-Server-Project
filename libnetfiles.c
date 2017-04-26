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
	/*TODO: set global variables with hostname, set flags*/
	return 0;
}
/*netopen: opens socket to the host, sends messege to 
server, recieves a negative # as server FD, close socket
and return the server FD(strictly negative)
	PARAM 
	@pathname: path to desired file in servers directory
	@flags:  O_WRONLY = 0
 			 O_RDONLY  = 1
 			 O_RDWR = 2
	RET: -1 if an error occurred, set ERRNO*/
int netopen(const char* pathname,int flags){
	struct hostent* server;
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char buf[256];
	bzero(buf,256);
	/*TODO: implement a way to pass variable in*/
	char*ip = "basic.cs.rutgers.edu";
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	/*open socket*/
	server = gethostbyname(ip);
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }
	switch(flags){
		case 0:
			/*server returns 0 upon fail*/
			strncat(buf,"W ",2);
			strncat(buf,pathname,strlen(pathname));
			ret = write(sockfd,buf,strlen(buf));
			if(ret == 0){
				puts("Error when writing to socket");
				close(sockfd);
				return -1;
			}
			bzero(buf,256);
			ret = read(sockfd,buf,256);
			if(ret == 0){
				puts("Error reading from socket");
				close(sockfd);
				return -1;
			}
			printf("%s\n",buf);
			close(sockfd);
			return ret;
		case 1:
			return -1;
		case 2:
			return -1;
		default: 
			return -1;
	}
}

