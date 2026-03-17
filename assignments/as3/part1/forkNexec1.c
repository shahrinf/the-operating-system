
#include <stdio.h> //for fgetc(),fgetc,fopen()
#include <stdlib.h> /* for exit*/


int main(){
	while(1){
		char* s;
		printf("enter a file name \n");
		scanf("%ms",&s);
		printf("%s\n",s);
		free(s);
	}


	return 0;
}


