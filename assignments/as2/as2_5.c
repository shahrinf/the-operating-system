#include <sys/wait.h>  //for wait()
#include <stdio.h> //for fgetc(),fgetc,fopen()
#include <unistd.h> //for fork()
#include <stdlib.h> /* for exit*/

int main(int argc, char* argv[]){
	if(argc<2){
		fprintf(stderr, "Usage %s <file name>\n", argv[0]);
	}

	/*FILE *
	  fopen(const char * restrict path, const char * restrict mode); restrict:tells compiler that there is no aliasing, i.e. ptr1!=ptr2 */
	FILE* fp=fopen(argv[1],"w");
	if(fp==NULL){
		perror("fopen() failed\n");
	}

	pid_t pid=fork();

	if(pid==0){ //child
		int i=0;
		while(i<1000){

			/* int
			   fputc(int c, FILE *stream); outputs to file pointed by stream
			 */
			putc(65,fp); //child process will write A
			i++;
		}
		printf("\n");
		exit(0);
	}

	else if(pid>0){
		int status;
		int i=0;
		while(i<1000){
			putc(66,fp); //parent process will write B
			i++;
		}
		printf("\n");

	}
	return 0;
}
