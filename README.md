# loyd.3

This program executes a parent process which spawns children processes to interact with 2 shared memory segments. A second clock and a nano second clock.
The critical section where the children interact with the shared memory is enforced by a message queue so that only one child is allowed in the critical section at one time.
This enforcement prohibits a race condition on the shared memory segment.

-----How to run-----

First run make

Executable will then be ./oss

if calling "./oss" without any additional arguments, the default will be 4 total children, 2 allowed to run at a time, and each child will increment the nano second clock by 1000000 once.

This is the same as calling "./oss -n 4 -s 2 -m 1"

you can call "./oss -h for help"


-----git log is below-----

Commits on Oct 23, 2022
Clock now adjusting correctly. In final stages

brendan loyd committed 38 minutes ago
 
Commits on Oct 21, 2022
critical section is enforced

brendan loyd committed 2 days ago
 
fixed msqid incrementing by 1

brendan loyd committed 2 days ago
 
Improvements made, still wip

brendan loyd committed 2 days ago
 
Commits on Oct 18, 2022
Implementing message queue, wip

brendan loyd committed 5 days ago
 
Commits on Oct 13, 2022
testing message queue in parent file

brendan loyd committed 10 days ago
 
Commits on Oct 11, 2022
setting up to add message queue

brendan loyd committed 12 days ago
 
Commits on Oct 10, 2022
initial commit

brendan loyd committed 13 days ago
 
Initial commit

@brendanloyd
brendanloyd committed 13 days ago
  


 
