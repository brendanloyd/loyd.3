#include "parent.h"

void terminateSigHandler(int sig) {
		printf("SIGINT signal encountered. Terminating.\n");
		printf("Clock value is: %d\n",*mem_ptr);
                shmdt(mem_ptr);
                kill(0, SIGQUIT);
}
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
	int option, totalChildProcesses = 4, clockIncrement;
	int childrenRunningAtOneTime = 2;
        int childProcessCounter;
        char *childNumber; //char arrays to send to child
        char *clock_Increment; //char arrays to send to child

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
				//if (totalChildProcesses > 18) {
                                //printf("Total number of child processes to be ran is: %i\n", totalChildProcesses);
				break;
			case 's' :
				childrenRunningAtOneTime = (atoi(optarg));
				if (childrenRunningAtOneTime > 18) {
				perror("Error: parent.c : Can't be more than 18 child processes running at one time.");
				exit(-1);

				}
				//printf("Total children that will be running at one time: %i\n", childrenRunningAtOneTime);
				break;
			case 'm' :
				clockIncrement = (atoi(optarg));
				//printf("Each child will increment the clock by: %i\n", clockIncrement);
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
        if(key = ftok("./parent.c" ,key_id) == -1) {
                perror("Key initialization failed.");
                exit(1);
        }

	//Setup id for message queue
        msqid = msgget(key,  0644|IPC_CREAT);
        
	
	//send message
	char message[200] = "Ready to enter critical section!";
        //strcpy(buf.mtext, "This is for jaggy");
	buf.mtype = 1;
	strcpy(buf.mtext, "Ready to enter critical section!");
	//sprintf(buf.mtext, "%s", message);
	//int len = strlen(buf.mtext);

        //if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        printf("MessageQid is : %d\n", msqid);
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                perror("msgsnd : parent.c ");
                exit(1);
        }
 
	int segment_id = shmget ( SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
	if (segment_id == -1) {
		perror("Error: parent.c : shared memory failed.");
	}


	  /* Attach the shared memory segment.  */
	char* tempPtr = (char *)(shmat(segment_id, 0, 0)); 
	int* shared_memory = (int*)(shmat(segment_id, 0, 0));
	if (shared_memory == NULL) {
		perror("Error: parent.c : shared memory attach failed.");
	} 
	
	  /* Set shared memory segment to 0  */
	*shared_memory = 0;
	mem_ptr = shared_memory;

	//Create nano clock in shared memory and set it to 0
	int *nano_clock = shared_memory + 1;
	*nano_clock = 0;
	
	clock_Increment = malloc(sizeof(clockIncrement));
	childNumber = malloc(sizeof(totalChildProcesses));
	//char *msq_id = malloc(sizeof(msqid));
	//sprintf(msq_id, "%d", msqid);

        sprintf(clock_Increment, "%d", clockIncrement);

		pid_t wpid;
		int status = 0;		
	        for (childProcessCounter = 0; childProcessCounter < totalChildProcesses; childProcessCounter++) {
                	//wait();
                	if(childProcessCounter == childrenRunningAtOneTime) {
                		//wait(NULL);
				while ((wpid = wait(&status)) > 0);
				childrenRunningAtOneTime *= 2;
			}
			pid_t childPid = fork(); // This is where the child process splits from the parent
                	sprintf(childNumber, "%d",(childProcessCounter + 1));
                	if (childPid == 0) {
                        	//char* args[] = {"./child", childNumber, clock_Increment, msq_id, 0};
                        	char* args[] = {"./child", childNumber, clock_Increment, 0};
				//execvp(args[0], args);
                        	execlp(args[0],args[0],args[1],args[2], args[3]);
				//execlp(args[0],args[0],args[1],args[2], args[3], args[4]);
                        	fprintf(stderr,"Exec failed, terminating\n");
                        	exit(1);
                	}
                }
	wait(NULL);
	//while ((wpid = wait(&status)) > 0);	
        printf("Clock value in seconds is: %d : NanoSeconds is : %d\nParent is now ending\n",*shared_memory, *nano_clock);
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      		perror("msgctl");
      		exit(1);
   	}
	shmdt(shared_memory);    // Detach from the shared memory segment
	shmctl( segment_id, IPC_RMID, NULL ); // Free shared memory segment shm_id
	return EXIT_SUCCESS; 
}
