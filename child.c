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
	int *nano_clock = shared_memory + 1;
	
	int forLoopIncrement = 0;
	for(forLoopIncrement; forLoopIncrement <= totalIncrements; forLoopIncrement++) {
		printf("worker_pid %d : Got in criticial section: Time: %d\n", getpid(), *shared_memory);
		*nano_clock += 1000000;
		if(*nano_clock >= 1000000000) {
			*nano_clock -= 1000000000;
			*shared_memory += 1;
		} 
		printf("worker_pid %d : Iteration %d  : Incrementing by %d : second %d nanosecond %d\n", getpid(), atoi(argv[1]), 1000000, *shared_memory, *nano_clock);
		printf("worker_pid %d : Leaving critical section: Time: %d\n", getpid(), *shared_memory);
	}

	shmdt(shared_memory);
	//sleep(3);
	return EXIT_SUCCESS;
}


