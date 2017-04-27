#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include "libnetfiles.h"

static struct hostent* init_IP = NULL;

int netserverinit(char* hostname){
	init_IP = gethostbyname(hostname);
	if(init_IP == NULL){
		return -1;
	}
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
	RET:
		neg int representing a serverFD 
		(error)-1 if an error occurred, set ERRNO*/
int netopen(const char* pathname,int flags){
	if(init_IP == NULL){
		puts("Need to initialize before calling netopen.");
		/*TODO: set errno*/
		return -1;
	}
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char buf[256];
	bzero(buf,256);
	/*TODO: implement a way to pass variable in*/
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }

	/*server returns 0 upon fail*/
	/*TODO: add mode to messege*/
	strncat(buf,"o ",2);	
	switch(flags){
		case 0 :
			strncat(buf,"w ",2);
			break;
		case 1 :
			strncat(buf,"r ",2);
			break;
		case 2 :
			strncat(buf,"r+ ",3);
			break;
		default : 
			puts("Error no flag inputed");
			return 0;
	}
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
	close(sockfd);
	return atoi(buf);
}
/*netread: opens socket to the host, sends messege to 
server, recieves a message with "(<nbytesread>(<buf>", nbytes 
has (( around it to use as delims
	PARAM 
	@fildes: FD number for serverFD you want to access<src>
	@buf: dest of the read command<dest>
	@nbytes: number of bytes you would like to read
	RET:
		non-neg int of #o of bytes actually read 
		(error)-1 if an error occurred, set ERRNO*/
ssize_t netread(int fildes,void*buf,size_t nbytes){
	if(init_IP == NULL){
		puts("Need to initialize before calling netread.");
		/*TODO: set errno*/
		return -1;
	}
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char* token;
	char buff[256];
	bzero(buff,256);
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }
	/*server returns 0 upon fail*/
	char tbuf[100];
	strncat(buff,"r ",2);
	sprintf(tbuf,"%d",fildes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	sprintf(tbuf," %zd",nbytes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	ret = write(sockfd,buff,strlen(buff));
	if(ret == 0){
		puts("Error when writing to socket");
		close(sockfd);
		return -1;
	}
	bzero(buff,256);
	ret = read(sockfd,buff,256);
	if(ret == 0){
		puts("Error reading from socket");
		close(sockfd);
		return -1;
	}
	//decode message ex."(100(abdcsjdnjndkf...
	token = strtok(buff,"(");
	ret = atoi(token);
	if(ret == 0){
		return ret;
	}
	token = strtok(NULL,"(");
	memcpy(buf,token,strlen(token));
	close(sockfd);
	return ret;
}
/*netwrite: opens socket to the host, sends messege to 
server, recieves a message with "(<nbyteswritten>(<buf>", nbytes 
has (( around it to use as delims
	PARAM 
	@fildes: FD number for serverFD you want to access<dest>
	@buf: what you want to write into file<src>
	@nbytes: number of bytes you would like to write
	RET:
		non-neg int of #o of bytes actually read 
		(error)-1 if an error occurred, set ERRNO*/
ssize_t netwrite(int fildes,const void*buf,size_t nbytes){
	if(init_IP == NULL){
		puts("Need to initialize before calling netwrite.");
		/*TODO: set errno*/
		return -1;
	}
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char* token;
	char buff[256];
	bzero(buff,256);
	/*TODO: implement a way to pass variable in*/
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }
	/*server returns 0 upon fail*/
	char tbuf[100];
	strncat(buff,"w ",2);
	sprintf(tbuf,"%d",fildes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	sprintf(tbuf," %zd ",nbytes);
	strncat(buff,tbuf,sizeof(tbuf));
	strncat(buff,buf,strlen(buf));
	bzero(tbuf,100);
	ret = write(sockfd,buff,strlen(buff));
	if(ret == 0){
		puts("Error when writing to socket");
		close(sockfd);
		return -1;
	}
	bzero(buff,256);
	ret = read(sockfd,buff,256);
	if(ret == 0){
		puts("Error reading from socket");
		close(sockfd);
		return -1;
	}
	//decode message
	ret = atoi(buff);
	close(sockfd);
	return ret;
}
/*netclose: close a given serverFD
	PARAM 
	@fd: FD number for serverFD you want to close
	RET:
		0 on success
		(error)-1 if an error occurred, set ERRNO*/
int netclose(int fd){
	if(init_IP == NULL){
		puts("Need to initialize before calling netwrite.");
		/*TODO: set errno*/
		return -1;
	}
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char buff[100];
	char temp[100];
	bzero(buff,100);
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }
	/*server returns 0 upon fail*/
	// make a mesege to send
    strncat(buff,"c ",2);
    sprintf(temp,"%d",fd);
    strncat(buff,temp,strlen(temp));
	ret = write(sockfd,buff,strlen(buff));
	if(ret == 0){
		puts("Error when writing to socket");
		close(sockfd);
		return -1;
	}
	bzero(buff,100);
	ret = read(sockfd,buff,100);
	if(ret == 0){
		puts("Error when writing to socket");
		close(sockfd);
		return -1;
	}
	//decode message
	ret = atoi(buff);
	close(sockfd);
	return ret;
}