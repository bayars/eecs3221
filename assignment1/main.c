#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

char *result(char *filenames){
    char *result = (char*)malloc(50 * sizeof(char));
    int fd[2];
    int nbytes;
    FILE *fp = fopen(filenames,"r");
    FILE *temp = fopen(filenames,"r");
    pid_t pid; 
    pipe(fd);
    pid=fork();
    char *buffer[80];
    if (pid < 0){
        fprintf(stderr,"Fork Failed");
    }else if( pid == 0 ){
        /* printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); */
        close(fd[0]);
        double max = findMax(fp);
        double min = findMin(temp);
        double sum = max+min;
        double dif = min-max;
        
        sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filenames,sum,dif,min,max);
        write(fd[1], result, 100* (strlen(result)+1));
        exit(0);
    }else{
        close(fd[1]);
        nbytes = read(fd[0],buffer, sizeof(buffer));
        printf("%s\n",buffer);
        wait(NULL);
    }
    return *buffer;

}


int main ( int argc, char *argv[] ){
    char *filenames[3] = {"./dataset1","./dataset2","./dataset3"};
    int len = sizeof(filenames)/sizeof(filenames[0]);
    result("./dataset1");
    result("./dataset2");
    result("./dataset3");
    return 0;
}

