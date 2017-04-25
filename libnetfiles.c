#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int netserverinit(char* hostname){
	server = gethostbyname(hostname);
	if(server = NULL){
		return -1;
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	int sockfd,n;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	int portno = 63666;
	char buffer[256];
	/*REMOVE: testing with hard coded ip*/
	char*ip = "basic.cs.rutgers.edu";

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
		error("Error opening socket.");
	server = netserverinit(ip);

	return 0;
}