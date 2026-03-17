#include <stdio.h>
#include <errno.h> /* for perror */
#include <sys/ipc.h> /* for shmget(),etc */
#include <sys/shm.h> /* for shmget(), shmat() */
#include <sys/types.h> /* for shmat() */
#include <unistd.h> /* sleep() */
#include <signal.h> /* for signal() */
#include <stdlib.h> /* for exit() */

typedef struct shstruct {
        int flag;
        int n;
}shstruct;

int shmid;

int fact(int n){
        int fac=1;
        for(int i=1;i<=n;i++){
                fac*=i;
        }
        return fac;
}


int main(int argc, char* argv[]){


        shmid=atoi(argv[1]);
        printf("shmid=%d\n",shmid);

        shstruct* shchild=shmat(shmid,NULL,0);
        if(shchild->flag==-1){
                perror("shmat() error\n");
                exit(1);
        }

        for(int i=1;i<=10;i++){
                sleep(1);
                if(shchild->flag==1){
                        shchild->flag=0;
                        printf("factorial %d = %d\n",shchild->n,fact(shchild->n));
                }
                else
                        i--;
        }

        return 0;
}


