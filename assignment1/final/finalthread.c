/*
 * =====================================================================================
 *
 *       Filename:  finalthread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2022-10-18 03:03:15 PM
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

#define SIZE 3

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

double findMax(FILE *fp){                               //Find Maximum value
    double first, second, N; 
    fscanf(fp,"%lf",&first);                            //Get first value base for comparison 
    while( fscanf(fp,"%lf",&second) != EOF){
        if ( second > first ){
            first = second;
        }
    }
    return first;
}


int main ( int argc, char *argv[] ){
    int k = 0;
    char result[200];
    static double totalmax=0.0;
    static double totalmin=100.0;
    char minBuf[50],maxBuf[50];
    char filename [100];
    for(int k = 1; k < argc; k++){  // For loop runs numbers of argument times
        int fd1[2];     // give filename to child
        int fd2[2];     // take back the calculated result
        int fd3[2];     // for move max
        int fd4[2];     // for move min

        pid_t proc;

        if(pipe(fd1) == -1 || pipe(fd2) == -1 || pipe(fd3) == -1 || pipe(fd4) == -1){
            fprintf(stderr, "Pipe Failed");     // If any pipe creation failed, print Pipe Failed
            return 1;
        }
        proc = fork();
        if ( proc < 0 ){
            fprintf(stderr,"fork failed");
            return 1;
        }else if( proc > 0 ){
            close(fd1[0]);
            write(fd1[1],argv[k],strlen(argv[k])+1);    // Load filename to fd1 pipe for send the filename
            close(fd2[1]);
            close(fd2[1]);
            read(fd2[0],result,200);    // read fd1 buffer which contains char array(result).
            close(fd2[0]);
            close(fd3[1]);
            read(fd3[0],minBuf,50);     // read minimum value from fd3 pipe
            close(fd4[1]);
            read(fd4[0],maxBuf,50);     // read maximum value from fd4 pipe 
            double maxTemp = strtod(maxBuf, NULL);  // Cast string to double for comparison
            double minTemp = strtod(minBuf, NULL);  // because pipe can only move char buffer or I couldn't make it
            if( totalmax < maxTemp){    // Find global minimum value
                totalmax = maxTemp;
            } 
            if( totalmin > minTemp){    // Find global minimum value
                totalmin = minTemp;
            }
            close(fd3[0]);
            close(fd4[0]);
            wait(NULL);    
        }else{
            close(fd1[1]);
            read(fd1[0],filename,200);  // Get filename in parent process
            FILE *fp = fopen(filename,"r");
            FILE *temp = fopen(filename,"r");
            double max = findMax(fp);
            double min = findMin(temp);
            double sum = max+min;
            double dif = min-max;
            if ( fp == NULL || temp == NULL ){  // If dataset files are note exists which given in argument
                sprintf(result,"File Pointer ERROR!!");
                exit(-2);
            }else{
                snprintf(minBuf, 150, "%lf", min);  // Convert back to double for comparison
                snprintf(maxBuf, 150, "%lf", max);  // Convert  back to double for comparison
                sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filename,sum,dif,min,max); // Concanater the result and move result buffer
            }
            close(fd1[0]);
            close(fd2[0]);
            write(fd2[1], result, strlen(result) + 1); // Load result char array to result buffer
            close(fd2[1]);
            close(fd3[0]);
            write(fd3[1], minBuf, strlen(minBuf)+1);  // Load minimum value in dataset to minBuf buffer as char array for pipe
            close(fd3[1]);   
            close(fd4[0]);
            write(fd4[1], maxBuf, strlen(maxBuf)+1); // Load maximum value in dataset to maxBuf buffer as char array for pipe
            close(fd4[1]);   
            exit(0);    // Exit from child process
        }
        printf("%s\n",result);
    }
    printf("MINIMUM=%lf MAXIMUM=%lf \n",totalmin,totalmax);
    return 0;
}

