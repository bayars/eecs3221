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

process processes[MAX_PROCESSES];                   /* Stores all process after reading CPULoad.dat file */
process_queue readyForProcessQueue;                 /* process Queue for ready to start processes */ /* Initiliazed with helper libraries*/
process_queue activeProcessQueue;                   /* Current active process queue*/ /* Initiliazed with helper libraries*/
int numberOfProcesses=0;                            /* total number of processes */ /* Initiliazed with helper libraries*/
int timeWatcher=0;                                  /* timeWatcher for keeping time for define start, end times.*/
int tempIndex=0;
process *tempProcess[MAX_PROCESSES];                    /* Temporary clone of a process. I used in the for get nex process in queue. Pointer for enque process  */
int tempProcessIndex=0;                             /* Used for previous temporary process array */
process *cpus[NUMBER_OF_PROCESSORS];
int lastpid;


int main(int argc, char *argv[]){
    int status=0;                                   /* Active state of process*/
    int totalWait=0, totalTurn=0, totalUtilzated=0;       /* I am using them for find total values.*/
    float averageWaitingTime;                       /* Average waiting time variable*/
    float averageTurnAround;                        /* Average turn around variable*/
    float averageUtilization;                       /* Average utilization variable*/
    
    initializeProcessQueue(&readyForProcessQueue);  /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/
        initializeProcessQueue(&activeProcessQueue);    /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/

    while(status = readProcess(&processes[numberOfProcesses])){
        if( status == 1 ){                          /* status if process created correctly. readprocess return check */
            numberOfProcesses++;                    /* Initiliaze all process with calling readProcess in helper library*/
        }
    }

    qsort(processes, numberOfProcesses, sizeof(process), compareByArrival);     /*  qsort is built-in method for sort a array. We are sorting processes array */

    while(true){                                    /* There is a break statement. It's looping until all lines proceed successfully  */
        while(tempIndex < numberOfProcesses && processes[tempIndex].arrivalTime <= timeWatcher){   /* until arrivaltime time of process smaller and equal than
                                                                                                       timeWatcher variable and check if tempIndex reached end of processes */
            tempProcess[tempProcessIndex++] = &processes[tempIndex++];  /* move processes to tempProcess array  */
            }
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
                }
            }
        }

        int ioSize= activeProcessQueue.size; /* Assign active process queue size to ioSize. I didnt assign in the for loop due to the queue size changes in loop.
                                                It's changing size in for loop and it causes infinite loop. */
        for(int i = 0; i < ioSize; i++){    /* Go over active process queue */
            process *removedProcessinQueue = activeProcessQueue.front->data;    /* Select the proccess for process data and assign removedprocess variable */
            dequeueProcess(&activeProcessQueue);    /* remove that process in the active process queue */
            if( removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step ==  removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].length){ /* if cpu process current burst step and lenght equal */
                removedProcessinQueue->currentBurst = removedProcessinQueue->currentBurst + 1;  /* increase currentburst variable in the removed process queue struct  */
                tempProcess[tempProcessIndex++] = removedProcessinQueue;    /* add removed process to tempProcess array */
            }else{
                enqueueProcess(&activeProcessQueue, removedProcessinQueue); /* if lenght is not same as step, add to queue for change the order of process.  */
            }
        }

        qsort(tempProcess, tempProcessIndex, sizeof(process *), compareByArrival); /* Sort the tempProcess array again for finalize ready to running process list. 
                                                                                      Currently, temp list shows ready to running in the tree graph in text book.*/
        for(int k = 0; k < tempProcessIndex; k++){ /* Go over temp process array for enqueue them to readyForProcessQueue queue with helper library */
            enqueueProcess(&readyForProcessQueue, tempProcess[k]);  /* Enqueue them to queue */
        }
        tempProcessIndex = 0;   /* After enqueue them set temp array index 0 for clean it to next while loop */
        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){  /* Go ove all cpus  */
            if(cpus[i] == NULL){    /* If iterated cpu number is not empty control */
                if(readyForProcessQueue.size == 0){  /* ready for process queue should not be empty. If queue is empty, then clear the iterated value of 
                                                        current cpu. Otherwise get data of process to cpu and dequeue current process in this queue */
                    cpus[i] = NULL;                 /* If ready for processes queue is empty, cpu empty.*/
                }else{
                    cpus[i] = readyForProcessQueue.front->data; /* copy the process data to cpu for process */
                    dequeueProcess(&readyForProcessQueue);  /* remove in this queue because it start to process. */
                }
            }
        }

        for(int i = 0; i < activeProcessQueue.size; i++){   /* Go over active process queue.*/
            process *removedProcessinQueue = activeProcessQueue.front->data; /* copy the active process data to temporary variable */
            dequeueProcess(&activeProcessQueue);    /* remove this value from active process queueu */
            (removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step)++;    /* Update the temporary variable with actual burst with temporary 
                                                                                               variable current burst and increase 1 in each loop.*/
            enqueueProcess(&activeProcessQueue, removedProcessinQueue); /* Enqueue again the temporary variable to active process queue. */
        }

        for(int i = 0; i < readyForProcessQueue.size; i++){ /* Go all over ready for Process queue  */
            process *removedProcessinQueue = readyForProcessQueue.front->data;  /* copy the readyForProcessQueue data to temporary variable */
            dequeueProcess(&readyForProcessQueue);  /* dequeue  the original readyforprocess queue */
            removedProcessinQueue->waitingTime++;   /* Increase waiting time for tmeporary variable because in each while loop, it's waiting for process */
            enqueueProcess(&readyForProcessQueue, removedProcessinQueue);   /* Put back the temporary variable to readyforprocess queue after update the waitingtime */
        }

        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){  /* Go over number of processors */
            if(cpus[i] != NULL){    /* if iterated cpu is not empty */
                cpus[i]->bursts[cpus[i]->currentBurst].step = (cpus[i]->bursts[cpus[i]->currentBurst]).step + 1; /* Increase 1 current burst step */
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

    printf("Average waiting time: %lf \nAverage turnaround: %lf \nCPU finished all process at: %d \nAverage CPU utilization %lf \nContextSwitch: %d \nLast process id: %d",averageWaitingTime,averageTurnAround, timeWatcher, (totalUtilzated / (float)timeWatcher)*100, 0, lastpid); 

    return 0;
}

