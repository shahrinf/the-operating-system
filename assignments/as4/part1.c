//Implementing a variable shared between a child process and its parent process and custom signal handler - Problem 1

#include <stdio.h>
#include <errno.h> /* for perror */
#include <sys/wait.h> /* for wait() */
#include <sys/ipc.h> /* for shmget() */
#include <sys/shm.h> /* for shmget(), shmat() */
#include <sys/types.h> /* for shmat() */
#include <unistd.h> /* for fork(),sleep() */
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

void releaseSHM(int signum){
	/* int shmctl(int shmid, int cmd, struct shmid_ds *buf); */
	int stat=shmctl(shmid,IPC_RMID,NULL);
	if(stat==-1){
		perror("shmctl() error\n");
	}
	printf("shared memory segment removed!\n");

	/*int kill(pid_t pid, int sig);*/
	stat=kill(0,SIGKILL); //pid=0;sig is sent to every process in the process group of the calling process
	if(stat==-1){
		perror("failed to kill process\n");
	}
}

int main(){
	typedef void (*sig_t)(int);

	/* sighandler_t signal(int signum, sighandler_t handler); 
	 * signum=SIG_DFL, default action; signum=SIG_IGN, signal ignored
	 */
	sig_t shandler=signal(SIGINT,releaseSHM);


	/* int shmget(key_t key, size_t size, int shmflg); 
	 * child process inherits shmid created before fork()
	 */

	shmid=shmget(IPC_PRIVATE,sizeof(shstruct),IPC_CREAT | 0777);
	if(shmid==-1){
		perror("shmget() error\n");
		exit(1);
	}



	pid_t pid=fork();
	if(pid<0){
		perror("fork() failed\n");
		exit(1);
	}

	if(pid==0){
		/* void *shmat(int shmid, const void *shmaddr, int shmflg); */
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

		exit(0);
	}

	else{
		shstruct* shparent=shmat(shmid,NULL,0);
		if(shparent->flag==-1){
			perror("shmat() error\n");
			exit(1);
		}

		shparent->flag=0;

		for(int i=1;i<=10;i++){
			if(shparent->flag==0){
				shparent->flag=1;
				shparent->n=i;
				printf("parent writes: %d\n",i);
			}
			else
				i--;
			sleep(1);
		}

		int status;
		wait(&status);

                //incase ctrl C not pressed
		int stat=shmctl(shmid,IPC_RMID,NULL);
		if(stat==-1){
			perror("shmctl() error\n");
		}
		printf("shared memory segment removed!\n");
		exit(0);
	}

}

