/*
Family Name:
Given Name:
Student Number:
EECS Login ID (the one you use to access the red server):
YorkU email address (the one that appears in eClass):
*/

// https://github.com/sarracini/CPU-Scheduling-Simulation
// https://github.com/yehongbing/CPU-Scheduler-Simulator

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
int tempIndex;
process *tempProcess[MAX_PROCESSES];		        /* Temporary clone of a process. I used in the for get nex process in queue. Pointer for enque process  */
int tempProcessIndex=0;                             /* Used for previous temporary process array */

void addNewIncomingProcess();
void runningToWaiting();
void readytoRunning();
void moveReadyQueue();
void Updatestatus();

// LAST variables
process *cpus[NUMBER_OF_PROCESSORS];
int lastpid;

int isAllIdle() {
	int i;
	int runningCPU = 0;
	for(i = 0; i < NUMBER_OF_PROCESSORS; i++) {
		if(cpus[i] != NULL) {
			runningCPU++;
		}
	}
	return runningCPU;
}

int main(int argc, char *argv[]){
    int status=0;                                   /* Active state of process*/
    int totalWait=0, totalTurn=0, totalUtilzated=0;       /* I am using them for find total values.*/
    float averageWaitingTime;                       /* Average waiting time variable*/
    float averageTurnAround;                        /* Average turn around variable*/
    float averageUtilization;                       /* Average utilization variable*/
    tempIndex = 0;

    
    initializeProcessQueue(&readyForProcessQueue);  /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/
	initializeProcessQueue(&activeProcessQueue);    /* Inilizae process queue to process ready */  /* Initiliazed with helper libraries*/

    while(status = readProcess(&processes[numberOfProcesses])){
        if( status == 1 ){
            numberOfProcesses++;
        }
    }
	qsort(processes, numberOfProcesses, sizeof(process), compareByArrival);


    while(true){
        //isAllIdle
        int numberofActivelyRunningCore =0;
        addNewIncomingProcess();
        runningToWaiting();
        readytoRunning();
        moveReadyQueue();
        Updatestatus();
        for( int i = 0; i < NUMBER_OF_PROCESSORS; i++) {
            if(cpus[i] != NULL) {
                numberofActivelyRunningCore++;
            }
        }
        if(numberofActivelyRunningCore == 0){
            totalUtilzated=0;
        }else{
            totalUtilzated++;
        }

        if(numberofActivelyRunningCore == 0 && (numberOfProcesses - tempIndex) == 0 && activeProcessQueue.size == 0){
            break;
        }
        printf("%d",tempIndex);
        timeWatcher++;
    }

    // getAllDataForDisplay
    for(int j = 0; j < numberOfProcesses; j++){
                printf("%d",processes[j].waitingTime);

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


void readytoRunning(){
    for(int i = 0; i < activeProcessQueue.size; i++){
        process *removedProcessinQueue = activeProcessQueue.front->data;
        dequeueProcess(&activeProcessQueue);
        if( removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].step ==  removedProcessinQueue->bursts[removedProcessinQueue->currentBurst].length ? true:false ){
            removedProcessinQueue->currentBurst = removedProcessinQueue->currentBurst + 1;
            tempProcess[tempProcessIndex++] = removedProcessinQueue;
        }else{
            enqueueProcess(&activeProcessQueue, removedProcessinQueue);
        }
    }
}

void Updatestatus(){
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
}

void moveReadyQueue(){
    qsort(tempProcess, tempProcessIndex, sizeof(process), compareByArrival);    
    for(int k = 0; k < tempProcessIndex; k++){
        enqueueProcess(&readyForProcessQueue, tempProcess[k]);
    }
    tempProcessIndex = 0;
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

}

void addNewIncomingProcess(){
                //
        while(tempIndex < numberOfProcesses && processes[tempIndex].arrivalTime <= timeWatcher) {
            tempProcess[tempProcessIndex++] = &processes[tempIndex++];
	    }
}

void runningToWaiting(){
    // runningToWaiting
    for( int i = 0; i < NUMBER_OF_PROCESSORS; i++){
        if(cpus[i] != NULL){
            if(cpus[i]->bursts[cpus[i]->currentBurst].step ==  cpus[i]->bursts[cpus[i]->currentBurst].length ? true:false) {
                cpus[i]->currentBurst = cpus[i]->currentBurst + 1;
                if((cpus[i])->currentBurst < (cpus[i])->numberOfBursts){
                    enqueueProcess(&activeProcessQueue, cpus[i]);
                    cpus[i] = NULL;
                }else{
                    cpus[i]->endTime = timeWatcher;
                    cpus[i] = NULL;
                }                    
            }
        }
    }
}