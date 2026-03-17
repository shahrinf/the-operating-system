/*Write a complete C program that reads for an array of n numbers (value of n is given by the user). The program then creates n child processes P1, P2, ..., Pn such that Pi, 1 ≤  i ≤  n, computes the ith largest number from the array and passes it to the parent process (say, P) through "exit(status)" system call and terminates. */

#include <stdio.h> /* for printf(), etc. */
#include <unistd.h> /* for pid_t fork(void);*/
#include <math.h>
#include <sys/wait.h>  /* for wait() */
#include <stdlib.h> /* for exit*/
#include <string.h> /* for reverse*/

int main(){

	int status;
	int n;
	printf("enter n\n");
	scanf("%d",&n);
	int arr[n];
	int soln[n]; 
	for(int l=0;l<n;l++){
		printf("enter element %d\n",l+1);
		scanf("%d",&arr[l]);
	}
	pid_t pid;
	for(int i=0;i<n;i++){
		pid=fork();
		//child process
		if(pid<0)
		{
			perror("fork failed\n");
			exit(1);
		}
		if(pid==0)
		{
			//for mth child,m-1 elements are already sorted
			int max=i;

			//finding ith largest no

			for(int k=i;k<n;k++){
				if(arr[k]>arr[max])
					max=k;
			}

			//this time each child terminates without forking
			exit(max); //parent will receive index of the ith largest element as the status exit code of the ith child                    
		}

		else{
			wait(&status);
//last byte of status is not 0 
			if(status&255)
			{
				perror("wait failed\n");
				exit(1);
			}
			//status is the index of the ith largest element
			//this time parent wont exit immediatedly after child terminates
			//swap the value at the returned index with the value at i

			int temp=arr[status>>8];
			arr[status>>8]=arr[i];
			arr[i]=temp;   
		}
	}

	printf("result:\n");
	for(int i=0;i<n;i++){
		printf("%d-th largest element : %d\n",i+1,arr[i]);
	}

	return(0);

}

