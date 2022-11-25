/*
Family Name: Bayar
Given Name: Safa
Student Number: 218295675
EECS Login ID (the one you use to access the red server): safabay
YorkU email address (the one that appears in eClass): safabay@my.yorku.ca
*/

#include "sch-helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>


process processes[MAX_PROCESSES];                   /* Stores all process after reading CPULoad.dat file */
process_queue readyForProcessQueue[3];              /* process Queue defination for three level feedback array.*/ /* Initiliazed with helper libraries*/
process_queue activeProcessQueue;                   /* Current active process queue*/ /* Initiliazed with helper libraries*/
int numberOfProcesses=0;                            /* total number of processes */ /* Initiliazed with helper libraries*/
int timeWatcher=0;                                  /* timeWatcher for keeping time for define start, end times.*/
int tempIndex=0;                                    /* Defination of temporary index number for using temporary assignments */
process *tempProcess[MAX_PROCESSES];                    /* Temporary clone of a process. I used in the for get nex process in queue. Pointer for enque process  */
int tempProcessIndex=0;                             /* Used for previous temporary process array */
process *cpus[NUMBER_OF_PROCESSORS];                /* Defined cpu's number of process with array. The cpu's will contains process for process the bursts*/
int lastpid;                                        /* Last pid variable definion for find last process id */
int contextSwitchNumber = 0;                        /* context switch number inited with 0 */
int quantumTime1;                                   /* Two quantum time variable initted due to stdin inputs*/
int quantumTime2;                                   /* We using quatum times for employing queue time */


void replaceBeginningofQueue(process_queue *readyForProcessQueue, process *process) {  /* At the third 3.c in assignment instruction tells:*/
/* a process preempted for servicing a higher priority queue should be placed in the beginning of its original ready queue*/
  process_node *node = createProcessNode(process);  /* Created temporary process node for move process to head. */
  if ( readyForProcessQueue->front == NULL ) {        /* if process front is null, then assing front and back 
                                                       side to temporary created process node */
      readyForProcessQueue->front = node;           /* That will make temporary process to first process queue */
      readyForProcessQueue->back = node;
  } else {                                          /* If process queue is not null (has some process node)*/
        node->next = readyForProcessQueue->front;   /* get temporary next and make next process node*/
        readyForProcessQueue->front = node;         /* and connect front side to node it self. That makes chain structure again */
  }
  readyForProcessQueue->size++;                     /* After add process node to beginning of the process queue, increase size */
}


int main(int argc, char *argv[]){
    int status=0;                                   /* Active state of process*/
    int totalWait=0, totalTurn=0, totalUtilzated=0;       /* I am using them for find total values.*/
    float averageWaitingTime;                       /* Average waiting time variable*/
    float averageTurnAround;                        /* Average turn around variable*/
    float averageUtilization;                       /* Average utilization variable*/
    int quantumTime1 = atoi(argv[1]);               /* Take inputs from standart inputs for first argument */
	int quantumTime2 = atoi(argv[2]);               /* Take inputs from standart inputs for second argument */
    tempProcessIndex =0;                            /* define temporary process index for tempProcess array  */
    tempIndex = 0;                                  /* Assign 0 to tempIndex that is keeping temporary index counter */

    // Initialize all processes on readyForProcessQueue for feedback queues. That will work for all feedback queues.
	for(int i = 0; i < 3; i++) {
			initializeProcessQueue(&readyForProcessQueue[i]); /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/
	}

    // Initialize activeProcess queue. This queue running uniqely. We don't need to implement for all feedback queues again.
	initializeProcessQueue(&activeProcessQueue); /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/

    // Read all of the process in dataset and increase numberOfPorcessess variable for watch number of processes.
    while((status = readProcess(&processes[numberOfProcesses]))){
        if( status == 1 ){                          /* status if process created correctly. readprocess return check */
            numberOfProcesses++;                    /* Initiliaze all process with calling readProcess in helper library*/
        }
    }

    qsort(processes, numberOfProcesses, sizeof(process), compareByArrival);     /*  qsort is built-in method for sort a array. We are sorting processes array */

    while(true){                                    /* There is a break statement. It's looping until all lines proceed successfully  */
        // everytime we reach the arrival time with timewatcher, we will add process to tempprocess. That will be used for next process scheduling.
        while(tempIndex < numberOfProcesses && processes[tempIndex].arrivalTime <= timeWatcher){   /* until arrivaltime time of process smaller and equal than
                                                                                                       timeWatcher variable and check if tempIndex reached end of processes */
            tempProcess[tempProcessIndex] = &processes[tempIndex++];  /* move processes to tempProcess array  */
            tempProcess[tempProcessIndex++]->quantumRemaining = quantumTime1;   /* Update temporary process array's quantumremaining time with current quantumTime1*/
        }
        
        process *temp1Process[NUMBER_OF_PROCESSORS];        /* Initialize new temporary process array to use in queues */
        int temp1Index=0;                                   /* Initialize new temporary process array to use in queue iterations */
        for( int i = 0; i < NUMBER_OF_PROCESSORS; i++){     /* go all over all processors with for loop */
            if(cpus[i] != NULL){                            /* if cpu (or core) not available*/
                if(cpus[i]->bursts[cpus[i]->currentBurst].step ==  cpus[i]->bursts[cpus[i]->currentBurst].length){ /* if cpu process current burst step and lenght equal */
                    cpus[i]->currentBurst = cpus[i]->currentBurst + 1;  /* increase current burst value in cpus struct  */
                    if((cpus[i])->currentBurst < (cpus[i])->numberOfBursts){    /* Check if process is ended or it is active process  */
                        enqueueProcess(&activeProcessQueue, cpus[i]);   /* If active process, add this process to active process queue with helper library*/
                        cpus[i] = NULL;         /* After added to  activeprocessqueue, clear the current cpu for next process */
                    }else{
                        cpus[i]->endTime = timeWatcher; /* if it's ended process, update the timeWatcher. We are using that in the next lines*/
                        cpus[i] = NULL;     /* If it's ended clear for next process. */
                    }                    
                }else if(cpus[i]->quantumRemaining == 0){   /* If current process's quantum time equal to 0. In other words, if process finished*/
                    temp1Process[temp1Index] = cpus[i];     /*Add the finished process to already creadted temp1Process for storing them */
                    temp1Index++;                           /* increase temprocess index next for add  new elements to empty indices. */
                    contextSwitchNumber++;                  /* We added this process to waiting */
                    cpus[i] = NULL;                         /* After process finished assign cpu to NULL */
                }else if( cpus[i]->quantumRemaining != 0 ) {    /* if process still continue, burst is not finished */
					if( readyForProcessQueue[0].size != 0 ) {   /* if ready process queue is empty */
						if( cpus[i]->currentQueue == 1 ) {      /* and if first feedback queue level is 1 */
							replaceBeginningofQueue(&readyForProcessQueue[cpus[i]->currentQueue], cpus[i]); /* Replace cpus[i] process to head 
                                                                                            in readprocess queueue in first level feedback queueu */
							cpus[i] = readyForProcessQueue[0].front->data;  /* Get fron data from readyforprocess queue front node data to process */
							dequeueProcess(&readyForProcessQueue[0]);   /* Dequeue the current node*/
							contextSwitchNumber++;  /* Increase the context switch number for process status changes in feedback queue level*/
						}
					}else if( readyForProcessQueue[1].size != 0 ) { /* if first feedback queue size is not equal to 0 */
						replaceBeginningofQueue(&readyForProcessQueue[2], cpus[i]); /* replace cpus[i] process with head process node of readyForProcessqueue */
						cpus[i] = readyForProcessQueue[1].front->data;  /* Get first fbq front node data to current process */
						dequeueProcess(&readyForProcessQueue[1]); /* and dequeue the first fbq*/
						contextSwitchNumber++;  /* Due to switch of context or process. Increase contextswitchNumber */
					}                
                } 
            }
            else if(cpus[i] == NULL){   /* if cpu's process is null*/
                for(int z = 0; z < 3; z++ ){ /* look all feedback queue levels */
                    if( readyForProcessQueue[z].size != 0 ) {     /* if the feedback level process queue is not empty */
                        cpus[i] = readyForProcessQueue[z].front->data; /* get queue's front node data and assign to current process */
                        dequeueProcess(&readyForProcessQueue[z]);   /* dequeue the readyForProcessQueue feedback queue */
                    }
                }
            }
        } 

        /* After dequeue process make sort again in temp1Process*/
        qsort(temp1Process, temp1Index, sizeof(process *), compareByArrival);

        for(int i=0; i < temp1Index; i++) { /* go through of temp1process. Run temp1index-1 time */
            if(temp1Process[i]->bursts[temp1Process[i]->currentBurst].step == quantumTime1) { /* if in first step is equal to quantumTime1 */
                temp1Process[i]->quantumRemaining = quantumTime2; /* Make remaining time to quantumTime2*/
                temp1Process[i]->currentQueue = 1;  /* assign current fbq level to 1*/
                enqueueProcess(&readyForProcessQueue[1], temp1Process[i]);  /* Enqueu this process to readyforprocess queue*/
            }else if(temp1Process[i]->bursts[temp1Process[i]->currentBurst].step == (quantumTime1+quantumTime2)) { /* otherwise, if process burst step is
                                                                                                                    equal to sum of quantumTimes */
                temp1Process[i]->currentQueue = 2;  /* asssgin to feedback queue level 2 */
                enqueueProcess(&readyForProcessQueue[2], temp1Process[i]); /* Enqueue this process to readyForProcessQueueu */
		    }
        }

        int ioSize= activeProcessQueue.size; /* Assign active process queue size to ioSize. I didnt assign in the for loop due to the queue size changes in loop.
                                                It's changing size in for loop and it causes infinite loop. */
        for(int i = 0; i < ioSize; i++){    /* Go over active process queue */
            process *removedProcessinQueue = activeProcessQueue.front->data;    /* Select the proccess for process data and assign removedprocess variable */
            dequeueProcess(&activeProcessQueue);    /* remove that process in the active process queue */
            if( removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step == removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].length){ /* if cpu process current burst step and lenght equal */
                removedProcessinQueue->currentBurst = removedProcessinQueue->currentBurst + 1;  /* increase currentburst variable in the removed process queue struct  */
                removedProcessinQueue->quantumRemaining = quantumTime1;   /* Update quantum remaining with quantum 1 due to fbq */
                tempProcess[tempProcessIndex++] = removedProcessinQueue;    /* add removed process to tempProcess array */
            }else{
                enqueueProcess(&activeProcessQueue, removedProcessinQueue); /* if lenght is not same as step, add to queue for change the order of process.  */
            }
        }

        qsort(tempProcess, tempProcessIndex, sizeof(process *), compareByArrival); /* Sort the tempProcess array again for finalize ready to running process list. 
                                                                                      Currently, temp list shows ready to running in the tree graph in text book.*/
        for(int k = 0; k < tempProcessIndex; k++){ /* Go over temp process array for enqueue them to readyForProcessQueue queue with helper library */
            tempProcess[k]->currentQueue = 0;   /* clear the process current queue. In the next update the readyForProcess Queue */
            enqueueProcess(&readyForProcessQueue[0], tempProcess[k]);  /* Enqueue them to queue */
        }

        tempProcessIndex = 0;   /* After enqueue them set temp array index 0 for clean it to next while loop */
        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){  /* Go ove all cpus  */
            if(cpus[i] == NULL){    /* If iterated cpu number is not empty control */
                if(readyForProcessQueue[0].size != 0){  /* ready for process queue should not be empty. If queue is empty, then clear the iterated value of 
                                                        current cpu. Otherwise get data of process to cpu and dequeue current process in this queue */
                    cpus[i] = readyForProcessQueue[0].front->data;  /* copy the process data to cpu for process */
				    dequeueProcess(&readyForProcessQueue[0]);   /* remove in this queue because it start to process. */
                }else if(readyForProcessQueue[1].size != 0){
                    cpus[i] = readyForProcessQueue[1].front->data;  /* copy the process data to cpu for process */
                    dequeueProcess(&readyForProcessQueue[1]);   /* remove in this queue because it start to process. */
                }else if(readyForProcessQueue[2].size != 0){
                    cpus[i] = readyForProcessQueue[2].front->data;  /* copy the process data to cpu for process */
                    dequeueProcess(&readyForProcessQueue[2]);   /* remove in this queue because it start to process. */
                }
            }
        }
        int  activeProcessQueueSize =  activeProcessQueue.size;
        for(int i = 0; i < activeProcessQueueSize; i++){   /* Go over active process queue.*/
            process *removedProcessinQueue = activeProcessQueue.front->data; /* copy the active process data to temporary variable */
            dequeueProcess(&activeProcessQueue);    /* remove this value from active process queueu */
            (removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step)++;    /* Update the temporary variable with actual burst with temporary 
                                                                                               variable current burst and increase 1 in each loop.*/
            enqueueProcess(&activeProcessQueue, removedProcessinQueue); /* Enqueue again the temporary variable to active process queue. */
        }

        for(int i = 0; i < 3; i++){ /* Go all over ready for Process queue  */
            for( int j = 0; j< readyForProcessQueue[i].size; j++){
                process *removedProcessinQueue = readyForProcessQueue[i].front->data;  /* copy the readyForProcessQueue data to temporary variable */
                dequeueProcess(&readyForProcessQueue[i]);  /* dequeue  the original readyforprocess queue */
                removedProcessinQueue->waitingTime++;   /* Increase waiting time for tmeporary variable because in each while loop, it's waiting for process */
                enqueueProcess(&readyForProcessQueue[i], removedProcessinQueue);   /* Put back the temporary variable to readyforprocess queue after update the waitingtime */
            }
        }

        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){  /* Go over number of processors */
            if(cpus[i] != NULL){    /* if iterated cpu is not empty */
                (cpus[i]->bursts[cpus[i]->currentBurst]).step = (cpus[i]->bursts[cpus[i]->currentBurst]).step + 1; /* Increase 1 current burst step */
                cpus[i]->quantumRemaining = cpus[i]->quantumRemaining - 1;
            }   /* This function increase step number for current burst. Because in each busy cpu causes burst step in all cpu */
        }

        int numberofActivelyRunningCore =0; /* Set actively running core number to 0 for find totalUtilized cpu as asked in question */
        for( int i = 0; i < NUMBER_OF_PROCESSORS; i++) {    /* go over all processor */
            if(cpus[i] != NULL) {   /* If cpu is not empty, it's busy. */
                numberofActivelyRunningCore++;  /* That mean that's utilized or used efficiently. Increase the created variable 1. */
            }
        }

        totalUtilzated += numberofActivelyRunningCore;  /* For find the total Utiliazed number of cpu. Sum tehm in total Utilized variable */

        if ( (numberofActivelyRunningCore == 0) && ( numberOfProcesses == tempIndex ) && (activeProcessQueue.size == 0 )){
            /* Most important and critical condition*/
            /* if all process are finished and current process number and temporary index is equal and there is not active process */
            /* Break the while loop and end this program for show final results. */
            break;
        }
        timeWatcher++; /* If it's passed break condition, increase timewatcher every while loop.*/
    }


    for(int j = 0; j < numberOfProcesses; j++){ /* Go over all processes */
        totalWait += processes[j].waitingTime;  /* Find the total waiting time in processes array, and move result to totalWait*/
        totalTurn += (processes[j].endTime - processes[j].arrivalTime); /* Find the total turnaround time, and move result to totalTurn  */
        if(processes[j].endTime == timeWatcher){ /* if the process end time is matching with timeWatcher. Get the pid for find last pid number  */
                lastpid = processes[j].pid; /* move last pid value to lastpid variable  */
        }
    }

    averageWaitingTime = totalWait / (float)numberOfProcesses; /* find average waiting time with totalwaiting time/number of processes  */
    averageTurnAround = totalTurn / (float)numberOfProcesses; /* find average turnaroung time with total turnaround time/ number of process  */
    averageUtilization = totalUtilzated / (float)timeWatcher; /* find average utilization with total utilized / last process timewatcher */

    printf("Average waiting time: %lf \nAverage turnaround: %lf \nCPU finished all process at: %d \nAverage CPU utilization %lf \nContextSwitch: %d \nLast process id: %d\n",averageWaitingTime,averageTurnAround, timeWatcher, (totalUtilzated / (float)timeWatcher)*100, contextSwitchNumber, lastpid); 

    return 0;
}

