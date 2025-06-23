//basic thread
//lets create two threads ,each doing 2*2 and 4\4 respectively, then lets add them

#include <stdio.h>
#include <pthread.h>

void* mult(void* arg1){
int res=*(int*)arg1;
res*=res;
*(int*)arg1=res;
return arg1;//dont do this->return &res;, because res is a local variable on the stack
}

void* div(void* arg1){
if(*(int*)arg1){
int res=*(int*)arg1;
res/=res;
*(int*)arg1=res;
}
return arg1;//dont do this->return &res;, because res is a local variable on the stack
}

int main(){
pthread_t t1,t2;

int arg1=2,arg2=4;

if(pthread_create(&t1,NULL,mult,&arg1)==0)
printf("success");
if(pthread_create(&t2,NULL,div,&arg2)==0)
printf("success");

printf("before joining %d %d ",arg1,arg2);
pthread_join(t1,NULL);
pthread_join(t2,NULL);
printf("after joining %d %d ",arg1,arg2);

return 0;
}
