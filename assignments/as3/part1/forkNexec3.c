//Write a C program which, in every iteration of an infinite-loop, reads the name of an executable file and creates a child process to execute it.

#include <sys/wait.h>  //for wait()
#include <stdio.h> //for fgetc(),fgetc,fopen()
#include <unistd.h> //for fork(),execve()
#include <stdlib.h> /* for exit*/


//File descriptors open in the calling process image remain open in the new process image(with an  exception)
int main(){
	while(1){
		char* s;
		printf("enter a file name \n");
		scanf("%ms",&s);  //turns out this doesnt work on my machine
		pid_t pid=fork();
		if(pid==0){
			//printf("%s\n",s);

			/*       int execve(const char *pathname, char *const argv[],
				 char *const envp[]);
				 pathname must be either a binary executable, or a script starting with a line of the form:

			 */
			char* argv[2];
			argv[0]=s;
			argv[1]=NULL;
			char* envp[]={NULL};
			execve(s,argv,envp);
			perror("execve failed\n"); //this line executes only if execve() has failed to spawn the new process
			printf("returned to calling process\n");
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

