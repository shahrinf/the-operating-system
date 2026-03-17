#include <stdio.h> /* for printf(), etc. */
#include <unistd.h> /* for pid_t fork(void);*/
#include <math.h>
#include <sys/wait.h>  /* for wait() */
#include <stdlib.h> /* for exit*/
#include <string.h> /* for reverse*/


int main(int argc, char* argv[]){

        int status;
        int n=argc-1;
        pid_t pid;
        printf("enter reverse executable file's name\n");
        char* reverse;
        scanf("%ms",&reverse); //dynamic allocation conversion specifier
        for(int i=0;i<n;i++){
                pid=fork(); // at each iteration no of processes doubles
                            //child process
                if(pid<0)
                        exit(1); //perror
                if(pid==0)
                {
                        printf("child %d execs\n",i+1);
                        char* arg[]={reverse,argv[i+1],NULL};
                        char* envp[]={NULL};
                        execve(reverse,arg,envp);
                        perror("execve failed\n");
                        exit(0);
                }
                else{
                        //parent process waits for child to terminate and then exits
                        wait(&status);
                }}

free(reverse);


        return(0);

}
