#include <sys/wait.h>  //for wait()
#include <stdio.h> //for fgetc(),fgetc,fopen()
#include <unistd.h> //for fork(),execve()
#include <stdlib.h> /* for exit*/


//File descriptors open in the calling process image remain open in the new process image(with an  exception)
int main(){
	while(1){
		char* s;
		printf("enter a file name \n");
		scanf("%ms",&s);
		pid_t pid=fork();
		if(pid==0){
			printf("%s\n",s);

			exit(0);
		}
		if(pid>0){
			int status;
			wait(&status);
			free(s);
		}

	}
	return 0;
}

