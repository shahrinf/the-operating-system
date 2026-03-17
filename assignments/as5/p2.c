//p2.c

#include <stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <unistd.h> /* for fork(2) sleep(3)*/
#include <stdlib.h> /* for exit(3) */

#define NO_OF_SEM 4  /* Number of semaphores needed in the semaphore-set to be created */

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


while(1){
//p2 section 1
//p2 acquires sem1
decr.sem_num=1;
status=semop(semid,&decr,1);  //this decr op wont be performed on sem as long as sem_val=0, so p2 cant acq sem1 until and unless p1 performs incr op on it, so until then p2 blocks
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }

printf("I am p2 section 1!\n");
sleep(2);
//release sem2 (will be used by p3)
incr.sem_num=2;
status=semop(semid,&incr,1);
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }

//acquire sem3 to proceed with section 2
decr.sem_num=3;
status=semop(semid,&decr,1);
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }

printf("I am p2 section 2!\n");
sleep(2);
//release sem0 so that p1 can restart the cycle
incr.sem_num=0; //to be performed on sem1
status=semop(semid,&incr,1);
if(status == -1) {
                perror("semop() failed");
                exit(1);
        }


}

}

