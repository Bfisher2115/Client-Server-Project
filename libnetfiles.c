#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "libnetfiles.h"


static struct hostent* init_IP = NULL;

int netserverinit(char* hostname){
	init_IP = gethostbyname(hostname);
	if(init_IP == NULL){
		perror("Error");
		return -1;
	}
	return 0;
}
/*netopen: opens socket to the host, sends messege to 
server, recieves a negative # as server FD, close socket
and return the server FD(strictly negative)
	PARAM 
	@pathname: path to desired file in servers directory
	@flags:  O_WRONLY = 1
 			 O_RDONLY  = 0
 			 O_RDWR = 2
	RET:
		neg int representing a serverFD 
		(error)-1 if an error occurred, set ERRNO*/
int netopen(const char* pathname,int flags){
	if(init_IP == NULL){
		perror("Error");
		return -1;
	}
	struct sockaddr_in serv_addr;
	socklen_t n;
	int ret,sockfd;
	char buf[256]; 
	char buf2[256];
	bzero(buf,256);
	bzero(buf2,256);
	/*open socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("Error");
		return -1;
	}
	/*set up port,connect to socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){
		perror("Error");
 		return -1;
    }
	/*server returns 0 upon fail*/
	/*TODO: add mode to messege*/
	strncat(buf,"o ",2);	
	strncat(buf,pathname,strlen(pathname));
	switch(flags){
		case 0:
			strncat(buf," 0",2);
			break;
		case 1:
			strncat(buf," 1",2);
			break;
		case 2:
			strncat(buf," 2",2);
			break;
	}
	/*Write to socket*/
	if(write(sockfd,buf,strlen(buf)) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	/*Read messege from socket*/
	bzero(buf,256);
	if(read(sockfd,buf,256) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	ret = atoi(buf);
	// if neg then success, got FD
	if(ret < 0){
		return ret;
	} 
	//otherwise, errno value sent  
	else{
		// TODO: set errno
		return -1;
	}
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
		perror("Error");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		perror("Error");
 		return -1;
    }
	// Create the messege to send to sever
	char tbuf[100];
	strncat(buff,"r ",2);
	sprintf(tbuf,"%d",fildes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	sprintf(tbuf," %zd",nbytes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	// send messege to server
	if(write(sockfd,buff,strlen(buff)) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	// read messege sent back from server
	bzero(buff,256);
	if(read(sockfd,buff,256) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	//decode message ex."S(100(abdcsjdnjndkf...
	token = strtok(buff,"(");
	// Read-Fail set val returned is Errno val
	if(buff[0] == 'F'){
		token = strtok(NULL,"(");
		ret = atoi(token);
		buf = NULL;
		close(sockfd);
		return -1;
	}
	// Success return number of bytes read, set buf
	else{
		token = strtok(NULL,"(");
		ret = atoi(token);
		token = strtok(NULL,"");
		memcpy(buf,token,strlen(token));
		close(sockfd);
		return ret;

	}
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
	char buff[256];
	char*token;
	bzero(buff,256);
	/*TODO: implement a way to pass variable in*/
	/*get socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("Error");
		return -1;
	}
	/*open socket*/
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		perror("Error");
 		return -1;
    }
	// send messege to the server
	char tbuf[100];
	bzero(tbuf,100);
	strncat(buff,"w ",2);
	sprintf(tbuf,"%d",fildes);
	strncat(buff,tbuf,sizeof(tbuf));
	bzero(tbuf,100);
	sprintf(tbuf," %zd ",nbytes);
	strncat(buff,tbuf,sizeof(tbuf));
	strncat(buff,buf,strlen(buf));
	bzero(tbuf,100);
	// write to the server
	if(write(sockfd,buff,strlen(buff)) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	// read messege back from the server
	bzero(buff,256);
	if(read(sockfd,buff,256) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	//decode message
	token = strtok(buff," ");
	if(token[0] == 'F'){
		errno = atoi(strtok(buff," "));
		perror("Error");
		return -1;
	}

	return atoi(strtok(buff," ")); 
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
	// open socket stream
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error opening socket.\n");
		return -1;
	}
	// create port , connect socket and port
	serv_addr.sin_family = AF_INET;
	bcopy((char*)init_IP->h_addr,(char*)&serv_addr.sin_addr.s_addr,init_IP->h_length);
	serv_addr.sin_port = htons(PORT);
	n = (socklen_t)sizeof(serv_addr);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,n) < 0){ 
 		printf("ERROR connecting\n");
 		return -1;
    }
	// make a mesege to send
    strncat(buff,"c ",2);
    sprintf(temp,"%d",fd);
    strncat(buff,temp,strlen(temp));
    // send messege to the server
	if(write(sockfd,buff,strlen(buff)) == -1){
		puts("Error when writing to socket");
		close(sockfd);
		return -1;
	}
	// recieve messege from the server
	bzero(buff,100);
	if(read(sockfd,buff,100) == -1){
		perror("Error");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	//decode message
	ret = atoi(buff);
	// set errno to one passed in
	if(ret != 0){
		errno = ret;
		perror("Error");
		return -1;
	}
	return 0;
}