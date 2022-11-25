#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#define SIZE 3

double findMin(FILE *fp){
    double first,second, N;
    fscanf(fp,"%lf",&first); 
    while( fscanf(fp,"%lf",&second) != EOF ){
        if ( second < first ){
            first = second;
        }
    }
    return first;
}

double findMax(FILE *fp){
    double first, second, N; 
    fscanf(fp,"%lf",&first);
    while( fscanf(fp,"%lf",&second) != EOF){
        if ( second > first ){
            first = second;
        }
    }
    return first;
}

void *threadFunc(void *filename){
    char *result = (char*)malloc(50 * sizeof(char));
    FILE *fp = fopen(filename,"r");
    FILE *temp = fopen(filename,"r");
    double max = findMax(fp);
    double min = findMin(temp);
    double sum = max+min;
    double dif = min-max;
    sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filename,sum,dif,min,max);
    printf("%s",result);
    pthread_exit(0);
}


int main ( int argc, char *argv[] ){
    char *filenames[SIZE] = {"./dataset1","./dataset2","./dataset3"};
    pthread_t tid[SIZE];
    pthread_attr_t attr[SIZE];
    for( int k = 0;k < SIZE;k++){
        pthread_attr_init(attr);
        pthread_create(&tid[k],&attr[k],threadFunc(filenames[k]),NULL);
        pthread_join(tid[k],NULL);

    }
    
    return 0;
}