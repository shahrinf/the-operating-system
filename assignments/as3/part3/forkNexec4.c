//Write a C program which, in every iteration of an infinite-loop, reads the name of an executable file along with cmdline args nd creates a child process to execute it.

#include <sys/wait.h>  //for wait()
#include <stdio.h> //for fgetc(),fgetc,fopen()
#include <unistd.h> //for fork(),execve()
#include <stdlib.h> /* for exit*/
#include <string.h>

#define MAXSIZE 100

int main(){
        char s[MAXSIZE];
        while(1){
                char* argv[MAXSIZE];
                int count=0; //no of arguments+1, eg. ls -l ,count=2
                pid_t pid=fork();
                if(pid==0){
                               printf("$ ");

                        /*       int execve(const char *pathname, char *const argv[],
                                 char *const envp[]);
                                 pathname must be either a binary executable, or a script starting with a line of the form:

                         */
                        //parsing the entered command
                        //create a temp buffer to read every word before copying that word to argv[]
                        //read to temp buffer using getchar() until a white spc is encountered, in which case make the last byte of temp buffer \0
                        //then just strcpy it to an argv, repeat until \n

                        char tempbuf[MAXSIZE];
                        char ch; int i;
                        while((ch=getchar())!='\n'){
                        i=0;
                        if(ch==' ')
                                continue;
                        tempbuf[i++]=ch;
                        while((ch=getchar())!=' '&& ch!='\n'){
                        tempbuf[i++]=ch;
                        }
                        tempbuf[i]='\0';
                        argv[count]=(char*)malloc(strlen(tempbuf)+1);
                        strcpy(argv[count++],tempbuf);
                        if(ch=='\n')
                                break;
                        }


                        argv[count]=NULL;
                        char* envp[]={NULL};
                        execve(argv[0],argv,envp);
                        perror("execve failed\n"); //this line executes only if execve() has failed to spawn the new process
                        printf("returned to calling process\n");
                        exit(1);
                }
                if(pid>0){
                        int status;
                        wait(&status);
                        while(count>0){
                        free(argv[--count]);
                        }
                }

        }
        return 0;
}

