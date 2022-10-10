#include "child.h"

int main(int argc, char** argv) {
	int increment = (atoi(argv[2]) * 1000000);
	int segment_id = shmget(SHMKEY, BUFF_SZ, 0777);
        if (segment_id == -1) {
                perror("Error: child.c : shared memory failed.");
        }	
	int* shared_memory = (int*)(shmat(segment_id, 0, 0));
	if (shared_memory == NULL) {
                perror("Error: child.c : shared memory attach failed.");
        }

	printf("Child_pid %d : Iteration %d  : Incrementing by %d\n", getpid(), atoi(argv[1]), increment);

	//shared_memory = shared_memory + atoi(argv[2]); 
	*shared_memory = (*shared_memory + increment);

	shmdt(shared_memory);
	//sleep(3);
	return EXIT_SUCCESS;
}


