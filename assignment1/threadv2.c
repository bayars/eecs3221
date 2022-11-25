#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#define SIZE 4

static double totalmax=0.0;
static double totalmin=100.0;

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
static char result[2000];

void *threadFunc(void *filename){
    FILE *fp = fopen(filename,"r");
    FILE *temp = fopen(filename,"r");
    double max = findMax(fp);
    double min = findMin(temp);
    double sum = max+min;
    double dif = min-max;
    if( totalmax < max){ 
        totalmax = max;
    } 
    if( totalmin > min){
        totalmin = min;
    }
    sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filename,sum,dif,min,max);
    printf("%s\n",result);
    pthread_exit(NULL);

}


int main ( int argc, char *argv[] ){
    pthread_t tid[SIZE];
    pthread_attr_t attr[SIZE];

    for( int k = 0;k < argc -1;k++){
        char *filename = argv[k+1];
        pthread_attr_init(&attr[k]);
        int isWorked = pthread_create(&tid[k],&attr[k],threadFunc,filename);
        if( isWorked ){
            printf("ERROR %d", isWorked);
            exit(-1);  
        }
        pthread_join(tid[k],NULL);
    }
    printf("MINIMUM=%lf MAXIMUM=%lf \n",totalmin,totalmax);
    pthread_exit(NULL);

    return 0;
}