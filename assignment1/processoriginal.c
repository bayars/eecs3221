#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
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


int main ( int argc, char *argv[] ){
    int k = 0;
    char result [200];
    for(int k = 1; k < argc; k++){
        int fd1[2];     // give filename to child
        int fd2[2];     // take back the calculated result

        pid_t proc;

        if(pipe(fd1) == -1 || pipe(fd2) == -1){
            fprintf(stderr, "Pipe Failed");
            return 1;
        }
        proc = fork();
        if ( proc < 0 ){
            fprintf(stderr,"fork failed");
            return 1;
        }else if( proc > 0 ){
            close(fd1[0]);
            write(fd1[1],argv[k],strlen(argv[k])+1);
            close(fd2[1]);
            wait(NULL);
            close(fd2[1]);
            read(fd2[0],result,200);
            printf("%s\n",result); // In parent print output
            close(fd2[0]);
        }else{
            char filename [100];
            char result [200];
            close(fd1[1]);
            read(fd1[0],filename,200);
            FILE *fp = fopen(filename,"r");
            FILE *temp = fopen(filename,"r");
            double max = findMax(fp);
            double min = findMin(temp);
            double sum = max+min;
            double dif = min-max;
            sprintf(result,"Filename %s SUM=%lf DIF=%lf MIN=%lf MAX=%lf",filename,sum,dif,min,max);
            close(fd1[0]);
            close(fd2[0]);
            write(fd2[1], result, strlen(result) + 1);
            close(fd2[1]);        
            exit(0);
        }
    }


    return 0;
}