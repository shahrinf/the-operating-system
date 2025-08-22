#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>           
#include <sys/wait.h>
#include <string.h>

#define SHM_SIZE (1024*sizeof(int))

void collatz(int n,int** ptr) {
	int * ptr2=*ptr;
	while (n != 1) {
		*ptr2=n;
		ptr2++;
		if (n % 2 == 0)
			n = n / 2;
		else
			n = 3 * n + 1;
	}
	*ptr2=1;
	ptr2++;
	*ptr=ptr2;
}

int main(int argc,char* argv[]){

	if(argc<2)
	{
		fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);           
		return 1;
	}

	char* name="collatz_shm";  // dont start the name with '/'

	int fd=shm_open(name,O_CREAT|O_RDWR,0666);
	if (fd == -1) {
		perror("shm_open");
		exit(1);
	}

	if (ftruncate(fd, SHM_SIZE) == -1) {
		perror("ftruncate");
		exit(1);
	}

	int* ptr=mmap(NULL,SHM_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if (ptr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	int* base_ptr=ptr;

	pid_t pid;
	pid=fork();

	if(pid==0){
		int n=atoi(argv[1]);

		collatz(n,&ptr);
		*ptr=-1;

		munmap(base_ptr,SHM_SIZE);
		shm_unlink(name);

		exit(0);
	}

	else if(pid>0){
		int status;
		waitpid(pid,&status,0);

		int d=*ptr;

		while(d!=-1){
			printf("%d ",d);
			ptr++;
			d=*ptr;
		}

		printf("\n");

		munmap(base_ptr,SHM_SIZE);
		shm_unlink(name);
	}

	return 0;

}



