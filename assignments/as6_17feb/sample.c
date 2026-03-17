//stupid 

#include <unistd.h>
#include  <stdio.h>
int main(){
pid_t pid=fork();

if(pid==0){
while(1){
printf("hey");}
}

if(pid>0){
printf("parent");
}

if(pid==0){
printf("closing child");
}

}

//does the execve child not inherit the fds?
