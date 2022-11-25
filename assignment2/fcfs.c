/*
Family Name:
Given Name:
Student Number:
EECS Login ID (the one you use to access the red server):
YorkU email address (the one that appears in eClass):
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
        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){
            if(cpus[i] == NULL){
                if(readyForProcessQueue.size == 0){
                    cpus[i] = NULL;
                }else{
                    cpus[i] = readyForProcessQueue.front->data;
                    dequeueProcess(&readyForProcessQueue);
                }
            }
        }

        //Update status
        for(int i = 0; i < activeProcessQueue.size; i++){
            process *removedProcessinQueue = activeProcessQueue.front->data;
            dequeueProcess(&activeProcessQueue);
            (removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step)++;
            enqueueProcess(&activeProcessQueue, removedProcessinQueue);
        }
        for(int i = 0; i < readyForProcessQueue.size; i++){
            process *removedProcessinQueue = readyForProcessQueue.front->data;
            dequeueProcess(&readyForProcessQueue);
            removedProcessinQueue->waitingTime++;
            enqueueProcess(&readyForProcessQueue, removedProcessinQueue);
        }
        for(int i = 0; i < NUMBER_OF_PROCESSORS; i++){
            if(cpus[i] != NULL){
                cpus[i]->bursts[cpus[i]->currentBurst].step = (cpus[i]->bursts[cpus[i]->currentBurst]).step + 1;
            }
        }

        //isAllIdle
        int numberofActivelyRunningCore =0;
        for( int i = 0; i < NUMBER_OF_PROCESSORS; i++) {
            if(cpus[i] != NULL) {
                numberofActivelyRunningCore++;
            }
        }

        totalUtilzated += numberofActivelyRunningCore;
        // printf("I am before break, total util : %d \n",totalUtilzated);

        // if ( numberofActivelyRunningCore == 0){
        //     totalUtilzated=0;
        // }else{
        //     totalUtilzated++;
        // } 


        if ( (numberofActivelyRunningCore == 0) && ((numberOfProcesses - tempIndex) == 0 ) && (activeProcessQueue.size == 0 )){
            /* for(int i = 0; i < numberOfProcesses; i++) { */
                                // printf("Process id #%d with total bursts: %d, now it's been %d bursts, current burst step: %d, total length to go %d\n", processes[i].pid,processes[i].numberOfBursts, processes[i].currentBurst, processes[i].bursts[processes[i].currentBurst].step,processes[i].bursts[processes[i].currentBurst].length);
                        /* } */
            break;
        }
        // printf("I am after break");
        // printf("Is all die: %d\n",tempIndex);

        // printf(" temp index: %d\n",tempIndex);
        // printf(" activeProcessQueue.size index: %d\n",activeProcessQueue.size);

        timeWatcher++;
    }

    // getAllDataForDisplay

    for(int j = 0; j < numberOfProcesses; j++){
                totalWait += processes[j].waitingTime;
                totalTurn += (processes[j].endTime - processes[j].arrivalTime);
                if(processes[j].endTime == timeWatcher){
                        lastpid = processes[j].pid;
                }
        }

    averageWaitingTime = totalWait / (float)numberOfProcesses;

        averageTurnAround = totalTurn / (float)numberOfProcesses;
        averageUtilization = totalUtilzated / (float)timeWatcher;

    printf("The average waiting time is: %.1f\n"
                        "The average turnaround time is: %.1f\n"
                        "The CPUs finished at: %d\nThe average cpu utilization is: %.2f%%\n"
                        "Total context switches: %d\n"
                        "The last process is: %d\n",averageWaitingTime,averageTurnAround, timeWatcher, (totalUtilzated / (float)timeWatcher)*100, 0, lastpid);


    return 0;
}
