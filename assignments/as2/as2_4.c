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
	FILE* fp=fopen(argv[1],"r");
	if(fp==NULL){
		perror("fopen() failed\n");
	}

	pid_t pid=fork();

	if(pid==0){ //child
		printf("child reading 10 chars from the file\n");
		//reading first 10 chars
		int i=0;
		while(1){
			/*int
			  fgetc(FILE *stream);*/
			putchar(65); //chars read by child process will be preceeded by A
			int ch=fgetc(fp);
			if(ch==EOF){
				char s[]="read error in child process\n";
				fputs(s,fp);
				break;
			}
			/*     int
			       putchar(int c); outputs to stdout*/
			ch=putchar(ch);
			i++;
		}
		printf("\n");
		exit(0);
	}

	else if(pid>0){
		int status;
		printf("parent reading 10 chars from the file\n");
		//reading first 10 chars
		int i=0;
		while(1){
			putchar(66); //chars read by child process will be preceeded by B
			int ch=fgetc(fp);
			if(ch==EOF){ 
				char s[]="read error in parent process\n";
				fputs(s,fp);
				break;
			}

			ch=putchar(ch);

			i++;
		}
		printf("\n");

	}
	return 0;
}
