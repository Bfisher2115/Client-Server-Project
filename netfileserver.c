#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#define PORT 63667
// size of hash table
#define SIZE 101


//the thread function
void serv_fun(int sockfd);
int sopen(char* msg);
char* sread(char* msg);
int swrite(char* msg);
int sclose(char* msg);
int hash(int key);
// child node
typedef struct ChildNode{
	int fd;
	// RD,WR,RDWR
	int flag;
	// UN,EX,TRAN
	int mode;
	// share key 
	struct ChildNode* next;
} node;
// bucket node for hash table
typedef struct bucketNode{
	int key;
	// collisions, same index diff key
	struct bucketNode* col;
	// share key 
	node* next;
} bucket;
// hash table
static bucket** h_table;

int sockfd, inSockfd;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;


int main(int argc, char const *argv[]){
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
    	perror("Error");
    	return 0;
    }
    int n = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int)) < 0){
    	puts("setsockopt(SO_REUSEADDR) failed");
    	perror("Error");
    	return 0;
    }
    /*set the feilds of the Sockaddr_in struct*/
    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    /*Htons: special function for converting to */
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    /*Names the socket if bind succeeds with newly created socket struct*/
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
    	perror("Error");
    	return 0;
    }
    /*listen stores incoming calls to socket in a queue*/
    listen(sockfd,10);
    /*accept takes top item off of queue in listen and creates a new connected socket
    and returns a FD*/
    clilen = (socklen_t)sizeof(cli_addr);
    // initialize your struct
    h_table = (bucket**)calloc(sizeof(bucket*),SIZE);
    //pthread_t thread_id;
    while( (inSockfd = accept(sockfd,(struct sockaddr*) &cli_addr, &clilen)) >0){
    	// TODO: threading
    	
    	serv_fun(inSockfd);
	
	// if( pthread_create( &thread_id , NULL ,  connection_handler , (void*)&client_sock) < 0){
 //            perror("could not create thread");
 //            return 1;
 //    }
	}
	return 0;
}
int sopen(char* msg){
	char buf2[256];
	char*token;
	bzero(buf2,256);
	int flag ,mode, index, fd;
		/*get pathname == buf2
		  flag	   == k*/
	token = strtok(msg," ");
	token = strtok(NULL," ");
 	strncat(buf2,token,strlen(token));
 	token = strtok(NULL," ");
 	flag = atoi(token);
 	token = strtok(NULL," ");
 	mode = atoi(token);

 	fd = open(buf2,flag);
 	if(fd == -1){
 		perror("Error");
 		return -1;
 	}
 	// get the key values
 	struct stat stats;
	fstat(fd,&stats);
	bzero(buf2,256);
	sprintf(buf2,"%ld%ld",stats.st_dev,stats.st_ino);
	int key = atoi(buf2);
	// get index using hash function
 	index = key % SIZE;
 	// ptr to index
 	bucket* bptr = h_table[index];
 	bucket*temp;
 	node* ptr;
 	// lock threads here;
 	// iterate across colissions in bucket
 	while(bptr != NULL){
 		// found matching key
 		if(bptr->key == key){
 			ptr = bptr->next;
 			// if in Tran then we cannot insert, if bptr != NULL we cant insert a TRAN mode FD
 			if(ptr->mode == 2  || mode == 2){
 				// TODO: return the aproriate errno value
 				return -1;
 			} 
 			if(flag == O_RDONLY){
 				// insert new node 
				// init node
				ptr = (node*)calloc(sizeof(node),1);
				ptr->fd = fd;
				ptr->mode = mode;
				ptr->flag = flag;
				ptr->next = bptr->next;
				bptr->next = ptr;
 				return fd;
 			}
 			switch(mode){
 				// UNREStricted
 				case 0:
 					while(ptr != NULL){
 						// exclusive write
 						if(ptr->mode == 1 && ptr->flag != O_RDONLY){
 							// cannot insert write, EXC write exists
 							// TODO: return the right errno
 							return -1;
 						}
 						ptr = ptr->next;
 					}
 					// add to front of the list
 					ptr = (node*)calloc(sizeof(node),1);
					ptr->fd = fd;
					ptr->mode = mode;
					ptr->flag = flag;
					ptr->next = bptr->next;
					bptr->next = ptr;
	 				return fd;
 				// EXclusive
 				case 1:
 					// EX write
 					while(ptr != NULL){
 						// if FD open in Write exists
 						if(ptr->flag != O_RDONLY){
 							// cannot insert a EXC write, another Fd is already open in Write mode
 							// TODO: return the right errno
 							return -1;
 						}
 						ptr = ptr->next;
 					}
 					// add to front of the list
 					ptr = (node*)calloc(sizeof(node),1);
					ptr->fd = fd;
					ptr->mode = mode;
					ptr->flag = flag;
					ptr->next = bptr->next;
					bptr->next = ptr;
	 				return fd;
 			}
 		}
 		// move to next collision
 		bptr = bptr->col;
 	}
 	// else make a new bucket node at bprt catches case where bucket unitiliazied
 	// init bucket
 	temp = (bucket*)calloc(sizeof(bucket),1);
 	temp->key = key;
 	temp->col = bptr;
 	// init node
	ptr = (node*)calloc(sizeof(node),1);
	ptr->fd = fd;
	ptr->mode = mode;
	ptr->flag = flag;
	// ptr arith
	temp->next = ptr;
	temp->col = bptr;
	h_table[index] = temp;
	return fd;
}

char* sread(char* msg){
	// decode messege
	int nbytes;
	int fd;
	char buf[256];
	char*token;

	token = strtok(msg," ");
	token = strtok(NULL," ");
	fd = -(atoi(token));
	token = strtok(NULL," ");
	nbytes = atoi(token);
	//check global state of fd
	//does it have Read/write access?

/*			k = fcntl(fd,F_GETFL);
	printf("%d\n",k);
	if(k == 32768){
		puts(strerror(errno));
		puts("file was opened in Write mode-cannot Read");
		k = write(sockfd,"(0(",2);
    	if (k < 0){
    		puts("ERROR writing to socket");
    	}
		break;
	}
*/
	//read from FD, set FD to front of file
	lseek(fd,0,SEEK_SET);
	bzero(buf,256);
	if(read(fd,buf,nbytes) == -1){
		perror("Error");
		return NULL;
	}
	// store the buf on the heap
	token = (char*)calloc(strlen(buf)+1,1);
	memcpy(token,buf,strlen(buf));
	return token;
}

int swrite(char* msg){
	int nbytes;
	int fd;
	char*token;
	//decode messege'
	token = strtok(msg," ");
	token = strtok(NULL," ");
	fd = -(atoi(token));
	token = strtok(NULL," ");
	nbytes = atoi(token);
	token = strtok(NULL,"");
	//check global state of fd
	//does it have Read/write access?

/*			k = fcntl(fd,F_GETFL);
	printf("%d\n",k);
	if(k == 00){
		puts(strerror(errno));
		puts("file was opened in Read mode-cannot Write");
		k = write(sockfd,"0",2);
    	if (k < 0){
    		puts("ERROR writing to socket");
    	}
		break;
	}
*/
	//read from FD, set FD to front of the file,
	// return the value
	lseek(fd,0,SEEK_SET);
	return write(fd,token,nbytes);
	
}

int sclose(char* msg){
	// decode messege
	char*token;
	int fd;
	token = strtok(msg," ");
	token = strtok(NULL," ");
	fd = -(atoi(token));
	//close the fd
	if(close(fd) == -1){
		perror("Error");
		return -1;
	}
	return 0;
}




void serv_fun(int sockfd){
	char*ret;
	int sFD;
	char* buffer;
	char buf2[256];
	int k = 0;
	struct stat stat1;
	buffer = (char*)calloc(256,1);
	/*Read fromk socket*/
 	if (read(sockfd,buffer,256) < 0){
 		perror("Error");
 		return;
 	}
 	printf("%s\n",buffer);
 	switch(buffer[0]){
 		/*sopen()*/
 		case 'o' :
 			/*call sopen() with rest of buffer*/
 			sFD = sopen(buffer);
 			/*Check return val*/
 			if( sFD == -1){
 				/*Write back the errno value*/
 				bzero(buf2,256);
 				sprintf(buf2,"%d",errno);
		    	if(write(sockfd,buf2,strlen(buf2)) < 0){
		    		perror("Error");
		   		}
		   		break;
			}
			fstat(sFD,&stat1);
			printf("st_dev: %zd   st_ino: %zd\n",stat1.st_dev,stat1.st_ino);
			// return the sFD
			// TODO: deal with making sFD negative
			bzero(buf2,256);
			// CHECK
		 	sprintf(buffer,"%d",-sFD);
		    if (write(sockfd,buffer,strlen(buffer)) < 0){
		    	perror("Error");
		    }
			break;
		/*sread()*/
		case 'r' :
			//call swrite and check return value
 			if( (ret = (sread(buffer))) == NULL){
 				/*Write back the errno value*/
 				bzero(buf2,256);
 				sprintf(buf2,"F(%d(",errno);
		    	if(write(sockfd,buf2,strlen(buf2)) < 0){
		    		perror("Error");
		   		}
		   		break;
			}
			// create messege "<S/F>(<nbytes>(<ret>"
			bzero(buf2,256);
			sprintf(buf2,"S(%zd(",strlen(ret));
			strncat(buf2,ret,strlen(ret));
			free(ret);
			if (write(sockfd,buf2,strlen(buf2))== -1){
		    	perror("Error");
		    }
 			break;
 		/*swrite()*/
		case 'w' :
			// call swrite()
			if( (k = swrite(buffer)) == -1){
				perror("Error");
				sprintf(buf2,"F %d",errno);
				if (write(sockfd,buf2,strlen(buf2)) == -1){
		    		perror("Error");
		    	}
		    	break;
			}
			//return amount of bytes written
			sprintf(buf2,"S %d",k);
			if (write(sockfd,buf2,strlen(buf2)) == -1){
		    		perror("Error");
		    	}
 			break;
 		/*sclose()*/
		case 'c' :
			// call sclose()
			// send back errno messege
			if(sclose(buffer) == -1){
				sprintf(buf2,"%d",errno);
				if(write(sockfd,buf2,strlen(buf2)) == -1){
					perror("Error");
				}
				break;
			}
			// write back success
			if(write(sockfd,"0",1) == -1){
				perror("Error");
			}
			break;
 	}
 	close(sockfd);
 	free(buffer);
 	return;
}