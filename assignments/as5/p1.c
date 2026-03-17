//p1.c

#include <stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <unistd.h> /* for fork(2) sleep(3)*/
#include <stdlib.h> /* for exit(3) */

/*we need a set of 4 sems,each except sem0 initialized to 0,and sem0 to 1, corresponding to each of p1,p2_1,p3,p2_2
first of all p1 acquires sem0 and when its done, it releases sem1 so that p2_1 can acquire it and similarly p2_1 goes on release sem2 for p3 then p3 releases p2_2 and atlast p2_2 releases p1 so the cycle can continue
*/
#define NO_OF_SEM 4  /* Number of semaphores needed in the semaphore-set to be created */

/* decr and incr defined below are initialized in the main() function */
struct sembuf decr; /* structure capturing the operation of decrementing a semaphore's value */
struct sembuf incr; /* structure capturing the operation of incrementing a semaphore's value */

int main(){
key_t mykey;
int semid;
int status;

mykey = ftok(".", 1);
if (mykey == -1) {
                perror("ftok() failed");
                exit(1);
        }

//describing the operations
        decr.sem_op = -1; // This operation to decrement the semaphore value
        decr.sem_flg = SEM_UNDO; // This operation is undone when the calling process terminates

        incr.sem_op = 1; // This operation to increment the semaphore value
        incr.sem_flg = SEM_UNDO; // This operation is undone when the calling process terminates


/* Used later for initializing the value of a semaphore */
        /* Reference: the manual for semctl() */
        union semun {
                int              val;    /* Value for SETVAL */
                struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
                unsigned short  *array;  /* Array for GETALL, SETALL */
                struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
        } setallArg;


semid = semget(mykey, NO_OF_SEM, IPC_CREAT | 0777);


/*how to initialize a semaphore
int
     semctl(int semid, int semnum, int cmd, ...);
for initialization of a sem semnum, cmd=SETVAL and setallArg.val=init_val, where union semun 'setvalArg' is the optional 4th argument in semctl used with specific cmds
for collective initialization of all sems in a set, cmd=SETALL, and setallArg.array[]=init_array
*/

//we have to initialize sem0 to 1, and the rest to 0
unsigned short array[4] = {1,0,0,0};
setallArg.array=array;
status=semctl(semid,0,SETALL,setallArg);
if(status == -1) {
                perror("semctl() failed");
                exit(1);
        }

while(1){
/*int
         semop(int semid, struct sembuf *sops, size_t nsops); //nsops-no of ops to be performed atomically on this sem
*/
//p1 acquires sem0
decr.sem_num=0;
status=semop(semid,&decr,1);
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }

printf("I am p1!\n");
sleep(2);
//release sem1 (will be used by p2_1)
incr.sem_num=1; //to be performed on sem1
status=semop(semid,&incr,1);
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }

}

}


