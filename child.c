#include "child.h"

int main(int argc, char** argv) {
	int totalIncrements = (atoi(argv[2]));
        
	//variables for message queue
	message buf;
        key_t key;
        int msqid = 0;
        const int key_id = 1234;

	//setup id for the second clock
	int segment_id = shmget(SHMKEY, BUFF_SZ, 0777);
        if (segment_id == -1) {
                perror("Error: child.c : shared memory failed.");
        }

	//setup id for the nano second clock
        int nano_segment_id = shmget ( NANOKEY, BUFF_SZ, 0777 | IPC_CREAT);
        if (nano_segment_id == -1) {
                perror("Error: parent.c : shared memory failed for nano clock");
        }
	
	//attach id for second clock
	int* shared_memory = (int*)(shmat(segment_id, 0, 0));
	if (shared_memory == NULL) {
                perror("Error: child.c : shared memory attach failed.");
        }

	//attach id for nano second clock;
        int* nano_clock = (int*)(shmat(nano_segment_id, 0, 0));
        if(nano_clock == NULL) {
                perror("Error: parent.c : shared memory for nano clock failed.");
        }

	//setup message queue	
	key = ftok("./parent.c", key_id);
        msqid = msgget(key, 0644|IPC_CREAT);

	
	int forLoopIncrement;
	for(forLoopIncrement = 0; forLoopIncrement < totalIncrements; forLoopIncrement++) {
		// recieve message if there is one, if not wait.
		msgrcv(msqid, &buf, sizeof(buf), 1, 0);
	
		//announce entry into critical section then go to sleep;
		printf("worker_pid %d : Got in criticial section: Time: %d\n", getpid(), *shared_memory);
		sleep(1);
	
		//increment nano second clock, adjust clocks if needed.
		*nano_clock += 1000000;
		if(*nano_clock >= 1000000000) {
			*nano_clock -= 1000000000;
			*shared_memory += 1;
		}
		//announce changes to shared memory variables then go back to sleeo.		
		printf("worker_pid %d : Iteration %d  : Incrementing by %d : second %d nanosecond %d\n", getpid(), atoi(argv[1]), 1000000, *shared_memory, *nano_clock);
		sleep(1);
		printf("worker_pid %d : Leaving critical section: Time: %d\n", getpid(), *shared_memory);

		//second message to message queue;
		strcpy(buf.mtext, "exiting critical section.");
        	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                	perror("msgsnd");
                	exit(1);
        	}
	}
	
	//detach shared memory and return success
	shmdt(shared_memory);
	return EXIT_SUCCESS;
}


