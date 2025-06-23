//lets check whether it makes sense to use semaphore and mutex together instead of just using mutex
//so, for macos, unnamed semaphores of type sem_t have been deprecated and hence i havent used sem_init,sem_destroy, insetad i have used macos'GCD (Grand Central Dispatch) semaphores
//Shared memory:
//unnamed sem :Threads only (in process memory)
//named sem:Threads & processes
// also, w and w/o semaphores output of the code :
//n=5,m=2
//with semaphore + mutex:
//  counter = 50000
//  time ≈ 0.125544 s
//mutex only:
//  counter = 50000
//  time ≈ 0.006808 s
//as one would expect
//i was just trying out these new topics learnt today

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <dispatch/dispatch.h>

int counter=0;
pthread_mutex_t lock;
//sem_t sem;
dispatch_semaphore_t sem;

void* loop(void* arg){
for(int i=0;i<10000;i++){
//sem_wait(&sem);
dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
pthread_mutex_lock(&lock);
counter++;
pthread_mutex_unlock(&lock);
//sem_post(&sem);
dispatch_semaphore_signal(sem);
}
return NULL;
}

void* loop2(void* arg){
for(int i=0;i<10000;i++){
pthread_mutex_lock(&lock);
counter++;
pthread_mutex_unlock(&lock);
}
return NULL;
}

int main(){
int n,m;
printf("enter no of threads and no of semaphore slots\n");
scanf("%d %d",&n,&m);

pthread_t t[5];
pthread_mutex_init(&lock,NULL);
//sem_init(&sem,0,m);
sem=dispatch_semaphore_create(m);
clock_t start,end;
double cpu_time_used;
start=clock();

for(int i=0;i<n;i++)
{
pthread_create(&t[i],NULL,loop,NULL);
}

for(int i=0;i<n;i++)
{
pthread_join(t[i],NULL);
}
end=clock();
cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;

printf("counter=%d\n",counter);
printf("CPU time used: %f seconds\n", cpu_time_used);

start=clock();
counter=0;
for(int i=0;i<n;i++)
{
pthread_create(&t[i],NULL,loop2,NULL);
}

for(int i=0;i<n;i++)
{
pthread_join(t[i],NULL);
}
end=clock();
cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;

printf("counter=%d\n",counter);
printf("CPU time used: %f seconds\n", cpu_time_used);

//sem_destroy(&sem);
return 0;
}




