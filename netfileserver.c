#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

//the thread function
void serv_fun(int sockfd);

int sockfd, inSockfd;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;

int main(int argc, char const *argv[]){
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
    	puts("Error occured when opening the socket.");
    	return 0;
    }
    int n = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int)) < 0){
    	puts("setsockopt(SO_REUSEADDR) failed");
    	return 0;
    }
    /*set the feilds of the Sockaddr_in struct*/
    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    /*Htons: special function for converting to */
    serv_addr.sin_port = htons(63666);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    /*Names the socket if bind succeeds with newly created socket struct*/
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
    	puts("Error on binding.");
    	return 0;
    }
    /*listen stores incoming calls to socket in a queue*/
    listen(sockfd,10);
    /*accept takes top item off of queue in listen and creates a new connected socket
    and returns a FD*/
    clilen = (socklen_t)sizeof(cli_addr);
    //pthread_t thread_id;
    while( (inSockfd = accept(sockfd,(struct sockaddr*) &cli_addr, &clilen)) >0){
    	serv_fun(inSockfd);
	
	/*if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0){
            perror("could not create thread");
            return 1;
        }*/


	}
	return 0;
}



void serv_fun(int sockfd){
	// read from socket
	// interpret string
	// perform specified action
		// open: write back neg of fd to given pathname
		// read: read specified number of bytes from FD, return amount of bytes read
		// write: write a specified number of bytes to FD, return amount of bytes written
		// close: close the socket, return 0 or -1
	FILE* fp;
	char*token;
	int fd,nbytes;
	char* mode = NULL;
	char* pathname = NULL;
	char buffer[256];
	char buf2[256];
	int k = 0;
	/*ZERO OUT*/
	bzero(buf2,256);
	bzero(buffer,256);
 	k = read(sockfd,buffer,255);
 	if (k < 0){
 		puts("ERROR reading from socket");
 	}
 	printf("%s\n",buffer);
 	token = strtok(buffer," ");
 	switch(token[0]){
 		/*NETOPEN()*/
 		case 'o' :
	 		token = strtok(NULL," ");
			mode = (char*)calloc(strlen(token)+1,1);
	 		strncat(mode,token,strlen(token));
	 		token = strtok(NULL," ");
	 		pathname = (char*)calloc(strlen(token)+1,1);
		 	strncat(pathname,token,strlen(token));
		 	if( (fp = fopen(pathname,mode)) < 0){
		 		/*TODO: since 0 is my error number on client side
		 		I might want to reopen if fp == 0*/
		 		/*TODO: set errno*/
		 		free(mode);
		 		free(pathname);
		 		puts("Error when opening file");
		 		k = write(sockfd,"0",2);
		    	if (k < 0){
		    		puts("ERROR writing to socket");
		    	}
		    	break;
	    	}
		    bzero(buffer,256);
		    /*return neg value of fd*/
		 	fd = -(fileno(fp));
		 	sprintf(buffer,"%d",fd);
		    k = write(sockfd,buffer,strlen(buffer));
		    if (k < 0){
		    	puts("ERROR writing to socket");
		    }
		    free(mode);
		 	free(pathname);
			break;
		/*NETREAD()*/
		case 'r' :
			//decode messege
			token = strtok(NULL," ");
			fd = -(atoi(token));
			printf("Incoming FD: %d\n",fd);
			token = strtok(NULL," ");
			nbytes = atoi(token);
			//check global state of fd
			//does it have Read/write access?
			k = fcntl(fd,F_GETFL);
			if(k == 32769){
				puts("file was opened in Write mode-cannot Read");
				k = write(sockfd,"(0(",2);
		    	if (k < 0){
		    		puts("ERROR writing to socket");
		    	}
				break;
			}
			//read from FD
			bzero(buffer,256);
			k = read(fd,buffer,nbytes);
			//return what you read to the socket in an message
			sprintf(buf2,"(%d(",k);
			strncat(buf2,buffer,strlen(buffer));
			k = write(sockfd,buf2,strlen(buf2));
			if (k < 0){
		    	puts("ERROR writing to socket");
		    }
 			break;
 		/*NETWRITE()*/
		case 'w' :
			//decode messege
			token = strtok(NULL," ");
			fd = -(atoi(token));
			token = strtok(NULL," ");
			nbytes = atoi(token);
			token = strtok(NULL," ");
			//check global state of fd
			//does it have Read/write access?
			k = fcntl(fd,F_GETFL);
			if(k == 32768){
				puts("file was opened in Read mode-cannot Write");
				k = write(sockfd,"0",2);
		    	if (k < 0){
		    		puts("ERROR writing to socket");
		    	}
				break;
			}
			//read from FD
			bzero(buffer,256);
			k = write(fd,token,nbytes);
			//return amount of bytes written
			sprintf(buf2,"%d",k);
			k = write(sockfd,buf2,strlen(buf2));
			if (k < 0){
		    		puts("ERROR writing to socket");
		    	}
 			break;
 		/*NETCLOSE()*/
		case 'c' :
			// decode messege <option> <fd>
			token = strtok(NULL," ");
			fd = -(atoi(token));
			//close the fd
			k = close(fd);
			if(k<0){
				puts("Error when closing file");
				k = write(sockfd,"-1",1);
				break;
			}
			k = write(sockfd,"0",1);
			break;
 	}
 	close(sockfd);
 	return;
}