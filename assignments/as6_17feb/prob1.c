#include <fcntl.h> //for open()
#include <stdio.h>
#include <unistd.h> /* for pipe() system call as stated in "man 2 pipe" */
/* for read() *
 * for write
 */
#include <stdlib.h>
#include <errno.h> /* Needed for perror */

#define readEnd 0 /* pipe() puts the "read fd" as 0th element in the int[] */
#define writeEnd 1 /* pipe() puts the "write fd" as 1th element in the int[] */

int main(int argc,char* argv[]) {

	if(argc<2){
		fprintf(stderr,"Usage ./a.out <file1> <file2> ...\n");
	}
	int flag; /* used to store the return value of system calls */
	int fds[2];
	int semid;

	char buf[100]; /* to be used by read(), write() */

	flag = pipe(fds);
	if (flag == -1 ) { // pipe() failed
		exit(79); // 79 is just some random number
	}


	pid_t pid=fork();	
	if(pid<0){
		perror("fork failed\n");
	}
	if(pid==0){


		int totalRead=0; //stores total no of bytes read from
		close(fds[writeEnd]);
		while(1){//till when will the child process keep reading
			flag=read(fds[readEnd],buf,sizeof(buf)-1);
			if(flag==-1){
				perror("read failed\n");
			}
			if(flag==0)
				break;
			totalRead+=flag;

		}

		printf("Read total %d characters \n",totalRead);
		close(fds[readEnd]);
	}

	if(pid>0){
		int fileno=1; //loops over the files
		int fd;
		close(fds[readEnd]);
		for(;fileno<argc;fileno++){
			fd=open(argv[fileno],O_RDONLY,0744);	
			if(fd==-1){
				perror("open failed\n");
				exit(1);
			}
			while(1){
				flag=read(fd,buf,sizeof(buf)-1);
				if(flag==-1){
					perror("read failed\n");
					break;	
				}	
				if(flag==0)
					break;
				write(fds[writeEnd],buf,flag);
			}
		close(fd);
		}
		close(fds[writeEnd]);
	}

}

