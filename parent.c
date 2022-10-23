#include "parent.h"

//signal handler function to kill program if ctrl c is encountered
void terminateSigHandler(int sig) {
		printf("SIGINT signal encountered. Terminating.\n");
		printf("Clock value is: %d\n",*mem_ptr);
                shmdt(mem_ptr);
                kill(0, SIGQUIT);
}

//signal handler function to timeout after a certain determined about of time
void timeoutSigHandler(int sig) {
	if(sig == SIGALRM) {
		printf("SIGALRM signal ecountered indicating a timeout. Terminating\n");
	 	printf("Clock value is: %d\n",*mem_ptr);
		shmdt(mem_ptr);	
		kill(0, SIGQUIT);	
	}

}

int main(int argc, char **argv) {

	//Variables for handling Getopt options and loop to run child processes
	int option, totalChildProcesses = 4, clockIncrement = 1;
	int childrenRunningAtOneTime = 2;
        int childProcessCounter;
        char *childNumber; //char array to send to child
        char *clock_Increment; //char array to send to child

	//Variables for signal handling
        signal(SIGTERM, terminateSigHandler);
        signal(SIGALRM, timeoutSigHandler);
        alarm(30);
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = terminateSigHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	//Variables for message queue
	message buf;
	key_t key;
	int msqid = 0;
	const int key_id = 1234;

	//Getopt options for program
	while ((option = getopt(argc, argv, "hn:s:m:")) != -1) {
                switch (option) {
                        case 'h' :
                                printf("To run this program please use the following format:\n");
                                printf("./oss [-h] [-n] [-s] [-m]\nWhere [-n] [-s] [-m] require arguments.\n");
				printf("Default process is: [./oss -n 4 -s 2]\n");
                                return 0;

                        case 'n' :
				totalChildProcesses = (atoi(optarg));
				break;

			case 's' :
				childrenRunningAtOneTime = (atoi(optarg));
				if (childrenRunningAtOneTime > 18) {
				perror("Error: parent.c : Can't be more than 18 child processes running at one time.");
				exit(-1);
				}
				break;

			case 'm' :
				clockIncrement = (atoi(optarg));
				break;

                        case '?':
                                printf("Driver : main : Invalid option.\n");
                                exit(-1);

                        default :
                                printf("Driver : main : Invalid argument\n");
                                return 0;
                
		}
        }

	//Setup key for message queue
	key = ftok("./parent.c",key_id);

	//Setup id for message queue
        msqid = msgget(key, 0644|IPC_CREAT);

	//store primer message	
	buf.mtype = 1;
	strcpy(buf.mtext, "Ready to enter critical section!");

	//Send message to message queue
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                perror("msgsnd : parent.c ");
                exit(1);
        }
 
	//get up seg id for second memory
	int segment_id = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
	if (segment_id == -1) {
		perror("Error: parent.c : shared memory failed.");
	}
	
	//set up seg id for nano second
	int nano_segment_id = shmget ( NANOKEY, BUFF_SZ, 0777 | IPC_CREAT);
	if (nano_segment_id == -1) {
		perror("Error: parent.c : shared memory failed for nano clock");
	}

	//attach memory segment for second clock
	int* shared_memory = (int*)(shmat(segment_id, 0, 0));
	if (shared_memory == NULL) {
		perror("Error: parent.c : shared memory attach failed.");
	} 

	//attach memory segment for nano second clock
	int* nano_clock = (int*)(shmat(nano_segment_id, 0, 0));
	if(nano_clock == NULL) {
		perror("Error: parent.c : shared memory for nano clock failed.");
	}
	
	/* Set shared memory segment to 0  */
	*shared_memory = 0;
	*nano_clock = 0;

	//this is for signal handling
	mem_ptr = shared_memory;

	//allocate space for variables to be passed to children
	clock_Increment = malloc(sizeof(clockIncrement));
	childNumber = malloc(sizeof(totalChildProcesses));
	
        sprintf(clock_Increment, "%d", clockIncrement);

	//For wait command
	pid_t wpid;
	int status = 0;		
	for (childProcessCounter = 0; childProcessCounter < totalChildProcesses; childProcessCounter++) {
                
		//wait if max children are running at one time
		if(childProcessCounter == childrenRunningAtOneTime) {
			while ((wpid = wait(&status)) > 0);
			childrenRunningAtOneTime *= 2;
		}

		//fork child and send arguments
		pid_t childPid = fork(); // This is where the child process splits from the parent
                sprintf(childNumber, "%d",(childProcessCounter + 1));
                if (childPid == 0) {
                        char* args[] = {"./child", childNumber, clock_Increment, 0};
                        execlp(args[0],args[0],args[1],args[2], args[3]);
                        fprintf(stderr,"Exec failed, terminating\n");
                        exit(1);
                }
        }

	//wait for all child processes to finish
	while ((wpid = wait(&status)) > 0);
	
	//print results	
        printf("Clock value in seconds is: %d : NanoSeconds is : %d\nParent is now ending\n",*shared_memory, *nano_clock);
	
	//detach message queue memory	
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      		perror("msgctl");
      		exit(1);
   	}
	
	//detach from the shared memory segment
	shmdt(shared_memory);

	//free shared memory segment shm_id
	shmctl( segment_id, IPC_RMID, NULL );
	return EXIT_SUCCESS; 
}
