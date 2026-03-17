#include <stdio.h> /* for printf(), etc. */
#include <string.h>

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
        if(argc<2){
        fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
        return 1;
        }
printf("original string : %s\n",argv[1]);
                        printf("reversed string: %s\n",strrev(argv[1]));
return 0;
}

