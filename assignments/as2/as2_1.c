/*Write a complete C program that reads n strings as command line arguments. That is, uses "int argc" and "char *argv[]" to read S1, S2, ..., Sn, when the program is executed as "./a.out S1 S2 ... Sn". The program then creates n child processes P1, P2, ..., Pn such that Pi, 1 ≤  i ≤  n, reverses the string Si and prints the reversed string.*/

#include <stdio.h> /* for printf(), etc. */
#include <unistd.h> /* for pid_t fork(void);*/
#include <math.h>
#include <sys/wait.h>  /* for wait() */
#include <stdlib.h> /* for exit*/
#include <string.h> /* for reverse*/


char* strrev(char* s){
	int l=strlen(s);
	char swap;
	for(int i=0;i<l/2;i++){
		swap=s[i];
		s[i]=s[l-i-1];
		s[l-i-1]=swap;
	}
	return s;
}

int main(int argc, char* argv[]){

	int status;
	int n=argc-1;
	pid_t pid;
	for(int i=0;i<n;i++){
		pid=fork(); // at each iteration no of processes doubles
			    //child process
		if(pid<0)
			exit(1); //perror
		if(pid==0)
		{
			printf("child %d\noriginal string : %s\n",i+1,argv[i+1]);
			printf("reversed string: %s\n",strrev(argv[i+1]));

			//reverse argv[i]
			//child process forks in next iteration then waits and later exits
		}
		else{
			//parent process waits for child to terminate and then exits
			wait(&status);
			exit(0);
		}
	}

	return(0);

}
