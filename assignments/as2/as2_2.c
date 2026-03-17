/*Write a complete C program that reads for an array of n numbers (value of n is given by the user). The program then creates n child processes P1, P2, ..., Pn such that Pi, 1 ≤  i ≤  n, computes and prints the ith largest number from the array.*/

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
	for(int l=0;l<n;l++){
		printf("enter element %d\n",l+1);
		scanf("%d",&arr[l]);
	}
	pid_t pid;
	for(int i=0;i<n;i++){
		pid=fork(); 
		//child process
		if(pid<0)
			exit(1);
		if(pid==0)
		{
			//for mth child,m-1 elements are already sorted
			int max=i; 

			//finding ith largest no

			for(int k=i;k<n;k++){
				if(arr[k]>arr[max])
					max=k;
			}
			printf("child %d\n: %d-th largest no =%d at idx %d\n",i+1,i+1,arr[max],max);
			int temp=arr[i];
			arr[i]=arr[max];
			arr[max]=temp;

		}                

		else{// wait(&status);
			exit(0);
	

	}
	}

	return(0);

}

