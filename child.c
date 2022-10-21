#include "child.h"

int main(int argc, char** argv) {
	int totalIncrements = (atoi(argv[2]));
	int segment_id = shmget(SHMKEY, BUFF_SZ, 0777);
        if (segment_id == -1) {
                perror("Error: child.c : shared memory failed.");
        }	
	int* shared_memory = (int*)(shmat(segment_id, 0, 0));
	if (shared_memory == NULL) {
                perror("Error: child.c : shared memory attach failed.");
        }

	message buf;
        key_t key;
	int msqid = 0;
	//int msqid = (atoi(argv[3]));
	const int key_id = 1234;

        if(key = ftok("./parent.c", key_id) == -1) {
                perror("Key initialization failed.");
                exit(1);
        }
        msqid = msgget(key, 0644|IPC_CREAT);

	int *nano_clock = shared_memory + 1;
	
	int forLoopIncrement;
	for(forLoopIncrement = 0; forLoopIncrement < totalIncrements; forLoopIncrement++) {
        	printf("worker_pid %d waiting entry. MessageQ id is: %d\n", getpid(),msqid);
		if(msgrcv((msqid-1), &buf, sizeof(buf), 1, 0) == -1) {
                	perror("msgrcv");
                	exit(1);
        	}
		printf("worker_pid %d : Got in criticial section: Time: %d\n", getpid(), *shared_memory);
		//sleep(1);
		*nano_clock += 1000000;
		if(*nano_clock >= 1000000000) {
			*nano_clock -= 1000000000;
			*shared_memory += 1;
		}
		 
		printf("worker_pid %d : Iteration %d  : Incrementing by %d : second %d nanosecond %d\n", getpid(), atoi(argv[1]), 1000000, *shared_memory, *nano_clock);
		printf("worker_pid %d : Leaving critical section: Time: %d\n", getpid(), *shared_memory);
		//buf.mtype = 1;
		strcpy(buf.mtext, "exiting critical section.");
		//sprintf(buf.mtext, "%s", message);
        	//int len = strlen(buf.mtext);
        	//if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                	perror("msgsnd");
                	exit(1);
        	}
	}

	shmdt(shared_memory);
	//sleep(3);
	return EXIT_SUCCESS;
}


