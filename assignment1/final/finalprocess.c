/*
 * =====================================================================================
 *
 *       Filename:  finalprocess.c
 *
 *    Description: i 
 *  G
 *
 *        Version:  1.0
 *        Created:  2022-10-18 03:03:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Safa Bayar (rection), safa@safabayar.tech
 *   Organization:  
 *
 * =====================================================================================
 */


/*
Family Name: Bayar
Given Name(s): Safa
Student Number: 218295675
EECS Login ID (the one you use to access the red server): safabay
YorkU email address (the one that appears in eClass): safabay@my.yorku.ca
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#define SIZE 3

static double totalmax=0.0;
static double totalmin=100.0;
static char result[200];

double findMin(FILE *fp){                               //Find Minimum value
    double first,second, N;
    fscanf(fp,"%lf",&first);                            //Get first value base for comparison
    while( fscanf(fp,"%lf",&second) != EOF ){
        if ( second < first ){
            first = second;
        }
    }
    return first;
}

double findMax(FILE *fp){                               //Find Minimum value
    double first, second, N;
    fscanf(fp,"%lf",&first);                            //Get first value base for comparison
    while( fscanf(fp,"%lf",&second) != EOF){
        if ( second > first ){
            first = second;
        }
    }
    return first;
}

void *threadFunc(void *filename){
    FILE *fp = fopen(filename,"r");
    FILE *temp = fopen(filename,"r");
    if ( fp == NULL || temp == NULL ){  // If dataset files are note exists which given in argument
        sprintf(result,"File Pointer ERROR!!");
            pthread_exit(NULL); // If file doesn't exist, exit from thread.
    }
    double max = findMax(fp);
    double min = findMin(temp);
    double sum = max+min;
    double dif = min-max;
    if( totalmax < max){    // Find global maximum value
        totalmax = max;
    }
    if( totalmin > min){    // Find global minimum value
        totalmin = min;
    }
    sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filename,sum,dif,min,max);
    pthread_exit(0);    // Everything went correctly ended this thread.

}


int main ( int argc, char *argv[] ){
    pthread_t tid[SIZE];
    pthread_attr_t attr[SIZE];

    for( int k = 0;k < argc -1;k++){
        pthread_attr_init(&attr[k]);    // Initted thread with for loop and pthread attribute variable
        int isWorked = pthread_create(&tid[k],&attr[k],threadFunc,argv[k+1]);   // Create thread and the t
        if( isWorked ){
            printf("ERROR %d", isWorked);
            exit(-1);
        }
        pthread_join(tid[k],NULL);
        printf("%s\n",result);
    }
    printf("MINIMUM=%lf MAXIMUM=%lf \n",totalmin,totalmax);

    return 0;
}

