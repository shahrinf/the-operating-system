/* sem0.c */
#include <stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <unistd.h> /* for fork(2) sleep(3)*/

#include <stdlib.h> /* for exit(3) */


#define NO_SEM	1 /* Number of semaphores needed in the semaphore-set to be created */

/* Pop and Vop defined below are initialized in the main() function */
struct sembuf Pop; /* structure capturing the operation of decrementing a semaphore's value */
struct sembuf Vop; /* structure capturing the operation of incrementing a semaphore's value */

#define P(sid) semop(sid, &Pop, 1); /* P(sid) decrements the value of the 1st semaphore of the set sid */
#define V(sid) semop(sid, &Vop, 1); /* V(sid) increments the value of the 1st semaphore of the set sid */

int main() {
	key_t mykey;
	pid_t pid;

	int semid;

	int status;


	/* Used later for initializing the value of a semaphore */
	/* Reference: the manual for semctl() */
	union semun {
		int              val;    /* Value for SETVAL */
		struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
	} setvalArg;



	/* Used later for semaphore operations */
	/* struct sembuf has the following fields */
	/* Reference: the manual for semop() */
	/*
	unsigned short sem_num;  // semaphore number
        short          sem_op;   // semaphore operation
        short          sem_flg;  // operation flags
	*/

	Pop.sem_num = 0; // This operation to be performed on the 0th (1st) semaphore of the set
	Pop.sem_op = -1; // This operation to decrement the semaphore value
	Pop.sem_flg = SEM_UNDO; // This operation is undone when the calling process terminates 

	Vop.sem_num = 0; // This operation to be performed on the 0th (1st) semaphore of the set
	Vop.sem_op = 1; // This operation to increment the semaphore value
	Vop.sem_flg = SEM_UNDO; // This operation is undone when the calling process terminates 

	// key_t ftok(const char *pathname, int proj_id);
	//mykey = ftok("/home/manas/Documents/Operating Systems/samples/sem/sem1.c", 1);
	mykey = ftok(".", 1);
	
	if (mykey == -1) {
		perror("ftok() failed");
		exit(1);
	}

	//       int semget(key_t key, int nsems, int semflg);
	semid = semget(IPC_PRIVATE, NO_SEM, IPC_CREAT | 0777);
	semid = semget(IPC_PRIVATE, NO_SEM, IPC_CREAT | 0777);
	semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
	semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
		// mykey - returns id for the semaphore set associated with mykey 
		// nsems - semaphore set shoud contain nsems semaphores
		// IPC_CREAT - Create the semaphore set if it is NOT already created
		// 0777 (octal) ie 111 111 111 (binary) - all rwx permission for UGO (user/owner, Group members, Others) 

	if(semid == -1) {
		perror("semget() failed");
		exit(1);
	}

	setvalArg.val = 1; /* for initializing the semaphore to 1 */
	// int semctl(int semid, int semnum, int cmd, ...);
	status = semctl(semid, 0, SETVAL, setvalArg); // Initilize the 0th (1st) semaphore of the set to 1
	if(status == -1) {
		perror("semctl() failed");
		exit(1);
	}

	pid = fork();
	if (pid == -1) {
		perror("fork() failed");
		exit(1);
	}

	if (pid == 0) {
		/* child process task */
		while(1) {
			P(semid);

			// Critical Section
			fprintf(stdout, "Child.\n");
			//sleep(2); /* sleeps for 2s */

			V(semid);
			sleep(2); /* sleeps for 2s */
		}
	} else {
		/* parent process task */
		while(1) {
			P(semid);

			// Critical Section
			fprintf(stdout, "Parent.\n");
			sleep(1); /* sleeps for 1s */

			V(semid);
			//sleep(1); /* sleeps for 1s */
		}
	}

}
