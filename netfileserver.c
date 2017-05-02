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
#define THREADS 10


//the thread function
void serv_fun(void* insockfd);
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
	long key;
	int size;
	// collisions, same index diff key
	struct bucketNode* col;
	// share key 
	node* next;
} bucket;
// hash table
static bucket** h_table;
// mutex lock
pthread_mutex_t lock;
// running count on # of active threads
int active_threads;

int main(int argc, char const *argv[]){
	int sockfd;
	int * inSockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	// initialize your struct
    h_table = (bucket**)calloc(sizeof(bucket*),SIZE);
	// open TCP stream
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
    	return 0;
    }
    // set socket opts
    int n = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int)) < 0){
    	return 0;
    }
    // set feilds of sockaddr_in
    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // bind socket with port
    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
    	return 0;
    }
    // queue up connections to the socket
    listen(sockfd,10);
    clilen = (socklen_t)sizeof(cli_addr);
    pthread_t worker_thread_id;
    // calloc for thread perameter
    inSockfd = malloc(sizeof(*inSockfd));
    while( (*inSockfd = accept(sockfd,(struct sockaddr*) &cli_addr, &clilen)) >0){

    	/*TESTING: use this to test struct functionality*/
    	// serv_fun(inSockfd);
		
		if( pthread_create( &worker_thread_id ,NULL,(void*)serv_fun,inSockfd ) < 0){
            return 1;
    	}
    	// increase thread count, wait until # < 10
    	active_threads++;
    	inSockfd = malloc(sizeof(*inSockfd));
    	while(active_threads == 10);
	}	
	return 0;
}
void serv_fun(void* insockfd){
	int sockfd = (*(int*)insockfd);
	char*ret;
	int sFD;
	char* buffer;
	char buf2[256];
	int k = 0;
	struct stat stat1;
	buffer = (char*)calloc(256,1);
	/*Read fromk socket*/
 	if (read(sockfd,buffer,256) < 0){	
 		free(buffer);
 		free(insockfd);
 		close(sockfd);
 		active_threads--;
 		return;
 	}
 	switch(buffer[0]){
 		/*sopen()*/
 		case 'o' :
 			/*call sopen() with rest of buffer*/
 			sFD = sopen(buffer);
 			// unlock after return
 			pthread_mutex_unlock(&lock);
 			/*Check return val*/
 			if( sFD == -1){
 				/*Write back the errno value*/
 				bzero(buf2,256);
 				sprintf(buf2,"%d",errno);
		    	write(sockfd,buf2,strlen(buf2));
		   		break;
			}
			fstat(sFD,&stat1);
			// return the sFD
			bzero(buf2,256);
			// CHECK
		 	sprintf(buffer,"%d",-sFD);
			write(sockfd,buffer,strlen(buffer));
			break;
		/*sread()*/
		case 'r' :
			//call swrite and check return value
			ret = sread(buffer);
 			if( ret == NULL){
 				/*Write back the errno value*/
 				bzero(buf2,256);
 				sprintf(buf2,"F(%d(",errno);
		    	write(sockfd,buf2,strlen(buf2));
		   		break;
			}
			// create messege "<S/F>(<nbytes>(<ret>"
			bzero(buf2,256);
			sprintf(buf2,"S(%zd(",strlen(ret));
			strncat(buf2,ret,strlen(ret));
			write(sockfd,buf2,strlen(buf2));
			free(ret);
 			break;
 		/*swrite()*/
		case 'w' :
			// call swrite()
			if( (k = swrite(buffer)) == -1){
				sprintf(buf2,"F %d",errno);
				write(sockfd,buf2,strlen(buf2));
		    	break;
			}
			//return amount of bytes written
			sprintf(buf2,"S %d",k);
			write(sockfd,buf2,strlen(buf2));
 			break;
 		/*sclose()*/
		case 'c' :
			// call sclose()
			// send back errno messege
			k = sclose(buffer);
			// unlock after return
			pthread_mutex_unlock(&lock);
			if( k == -1){
				sprintf(buf2,"%d",errno);
				write(sockfd,buf2,strlen(buf2));
				break;
			}
			// write back success
			write(sockfd,"0",1);
			break;
 	}
 	// book keeping
 	free(buffer);
 	free(insockfd);
 	close(sockfd);
 	active_threads--;
 	return;
}
int sopen(char* msg){
	char buf2[256];
	char*token;
	struct stat stats;
	int flag ,mode, index, fd;
		/*get pathname == buf2
		  flag	   == k*/
	bzero(buf2,256);
	token = strtok(msg," ");
	token = strtok(NULL," ");
 	strncat(buf2,token,strlen(token));
 	token = strtok(NULL," ");
 	flag = atoi(token);
 	token = strtok(NULL," ");
 	mode = atoi(token);
 	// CRITICAL SECTION: TRYING TO OPEN FILE, INSERT TO DATA STRUCT
 	pthread_mutex_lock(&lock);
 	// get the key values
 	fd = open(buf2,flag);
 	if(fd == -1){
 		return -1;
 	}
	fstat(fd,&stats);
	bzero(buf2,256);
	sprintf(buf2,"%ld%ld",stats.st_dev,stats.st_ino);
	long key = strtol(buf2,&token,10);
	// get index using hash function
 	index = key % SIZE;
 	// ptr to index
 	bucket* bptr = h_table[index];
 	bucket*temp;
 	node* ptr;
 	// iterate across colissions in bucket
 	while(bptr != NULL){
 		// found matching key
 		if(bptr->key == key){
 			ptr = bptr->next;
 			// if in Tran then we cannot insert, if bptr != NULL we cant insert a TRAN mode FD
 			if(ptr->mode == 2  || mode == 2){
 				errno = EACCES;
 				close(fd);
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
				bptr->size++;
 				return fd;
 			}
 			switch(mode){
 				// UNREStricted
 				case 0:
 					while(ptr != NULL){
 						// exclusive write
 						if(ptr->mode == 1 && ptr->flag != O_RDONLY){
 							// cannot insert write, EXC write exists
 							errno = EACCES;
 							close(fd);
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
					bptr->size++;
	 				return fd;
 				// EXclusive
 				case 1:
 					// EX write
 					while(ptr != NULL){
 						// if FD open in Write exists
 						if(ptr->flag != O_RDONLY){
 							// cannot insert a EXC write, another Fd is already open in Write mode
 							errno = EACCES;
 							close(fd);
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
					bptr->size++;
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
 	temp->size++;
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
	int x;

	token = strtok(msg," ");
	token = strtok(NULL," ");
	fd = -(atoi(token));
	token = strtok(NULL," ");
	nbytes = atoi(token);
	//check global state of fd
	//does it have Read/write access?
	//read from FD, set FD to front of file
	lseek(fd,0,SEEK_SET);
	bzero(buf,256);
	errno = -1;
	pthread_mutex_lock(&lock);
	x = read(fd,buf,nbytes);
	if( x == -1){
		errno = EACCES;
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
	//read from FD, set FD to front of the file,
	// return the value
	lseek(fd,0,SEEK_SET);
	pthread_mutex_lock(&lock);
	return write(fd,token,nbytes);
	
}

int sclose(char* msg){
	char*token;
	int fd;
	char buf2[256];
	int index;
	// decode messege
	token = strtok(msg," ");
	token = strtok(NULL," ");
	fd = -(atoi(token));
	// get the key values
 	struct stat stats;
	fstat(fd,&stats);
	bzero(buf2,256);
	sprintf(buf2,"%ld%ld",stats.st_dev,stats.st_ino);
	long key = strtol(buf2,&token,10);
	// get index using hash function
 	index = key % SIZE;
 	// ptr to index
 	bucket* bptr = h_table[index];
 	node* ptr = NULL;
 	node* prev = NULL;
 	// lock mutex
 	pthread_mutex_lock(&lock);
 	while(bptr != NULL){
 		if(bptr->key == key){
 			ptr = bptr->next;
 			// case if there is one fd in list
 			if(bptr->size == 1){
 				if(ptr->fd == fd){
 					// found file
 					close(fd);
 					// delete bptr+node
 					free(ptr);
 					h_table[index] = bptr->col;
 					free(bptr);
 					return 0;
 				}
 			}
 			// other wise check nodes
 			while(ptr != NULL){
 				if(ptr->fd == fd){
 					// del first node in LL
 					if(prev == NULL){
 						bptr->next = ptr->next;
		 				free(ptr);
		 				bptr->size--;
		 				close(fd);
		 				return 0;
		 			}
		 			// del from list anywhere else
		 			else{
		 				prev->next = ptr->next;
		 				bptr->size--;
		 				free(ptr);
		 				close(fd);
		 				return 0;
		 			}
 				}
 				prev = ptr;
 				ptr = ptr->next;
 			}
 		}
 		bptr = bptr->col;
 	}
 	// failed to find it
 	// TODO: set errno value
 	errno = EBADF;
	return -1;
}

