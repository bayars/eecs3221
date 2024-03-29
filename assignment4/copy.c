#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>

#define TEN_MILLIS_IN_NANOS 10000000

typedef struct{
    char data;
    off_t offset;
}Data;

// nested struct with Data, and it stores thread integer variable
typedef struct{
    Data data;
    int threadNumber;
    int bufferSize;
    off_t end_position;
}ThreadData;

off_t end_position; 
struct timespec t;
Data *buffer;
int insertPointer=0;
int removePointer=0;
// int *bufSize=51;


pthread_mutex_t mutex;
sem_t empty;
sem_t full;


int inputFile;
int tempinputFile;
int outputFile;
int logFile;
FILE *fp;


//write to log file
void writeToLog(char *operation,char *operationName, int threadNumber, Data bufferItem, int i){
    fprintf(fp, "%s %s%i 0%ld B%d I%d\n", operation, operationName, threadNumber, bufferItem.offset, bufferItem.data, i); 
}

void nsleep(){
    t.tv_sec = 0;
    t.tv_nsec =  rand() % TEN_MILLIS_IN_NANOS;
    nanosleep(&t, NULL);
}

void *producer( void * param){
    ThreadData *threadData = (ThreadData *) param;
    Data item = threadData->data;
    while (1){
        printf("original end position %ld, item position %ld\n",threadData->end_position,item.offset);
        nsleep();
        pthread_mutex_lock(&mutex);
        if(threadData->end_position == item.offset || item.data == '\n' || item.data == EOF){
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
            exit(0);
        }
        if((item.offset = lseek(inputFile,0,SEEK_CUR)) < 0){
            pthread_mutex_unlock(&mutex);
            fprintf(stderr, "Error: lseek failed1\n");
            exit(1);
        }
        if( read(inputFile, &item.data, 1) < 0){
            printf("read_byte PT%ld: EOF pthread_exit(0)\n", pthread_self());
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        writeToLog("read_byte","P",threadData->threadNumber, item, -1);
        pthread_mutex_unlock(&mutex);
        nsleep();
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[insertPointer] = item;
        writeToLog("produce","P",threadData->threadNumber, item, insertPointer);
        insertPointer++;
        insertPointer = (insertPointer) % threadData->bufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer( void * param){
    ThreadData *threadData = (ThreadData *) param;
    Data item = threadData->data;
    while (1){
        nsleep();
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        item = buffer[removePointer];
        writeToLog("consume","C", threadData->threadNumber, item, removePointer);
        removePointer++;
        removePointer = (removePointer) % threadData->bufferSize;
        if(write(outputFile, &item.data, 1) < 1){
            pthread_mutex_unlock(&mutex);
            fprintf(stderr, "Error: write failed\n");
            exit(1);
        }
        writeToLog("write_byte","C",threadData->threadNumber, item, -1);
        sem_post(&empty); 
        pthread_mutex_unlock(&mutex);
        nsleep();
    }
}


int main(int argc, char *argv[]) {
    ThreadData threaddata;

    int inThreadNumber = atoi(argv[1]);
    int outThreadNumber = atoi(argv[2]);
    char *inputFileName = argv[3];
    char *outputFileName = argv[4];
    int bufferSize = atoi(argv[5]);
    char *logFileName = argv[6];

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, bufferSize);
    pthread_mutex_init(&mutex, NULL);

    if( inThreadNumber < 1 ){
        fprintf(stderr, "Error: Invalid number of input threads\n");
        exit(1);
    }

    if( outThreadNumber < 1 ){
        fprintf(stderr, "Error: Invalid number of output threads\n");
        exit(1);
    }

    if( !(fopen(inputFileName, "r")) ){
        fprintf(stderr, "Error: file not exist\n");
        exit(1);
    }

    if( !(fopen(outputFileName, "w")) ){
        fprintf(stderr, "Error: file not exist\n");
        exit(1);
    }

    if( bufferSize < 1 ){
        fprintf(stderr, "Error: Invalid buffer size\n");
        exit(1);
    }

    if( !(fopen(logFileName, "r")) ){
        fprintf(stderr, "Error: logfile not exist\n");
        exit(1);
    }

    outputFile = open(outputFileName, O_WRONLY | O_CREAT, 0666);
    inputFile = open(inputFileName, O_RDONLY,0666);
    tempinputFile = open(inputFileName, O_RDONLY,0666);
    fp = fopen(logFileName , "w");

    buffer = malloc( bufferSize * sizeof(Data) );          
    off_t end_positionofinputfile = lseek(tempinputFile, 1, SEEK_END);
    threaddata.bufferSize=bufferSize;
    threaddata.end_position=end_positionofinputfile - 1;

    pthread_t inThreads[inThreadNumber], outThreads[outThreadNumber];

    for( int x = 0; x < inThreadNumber; x++){
        threaddata.threadNumber=x;
        if( pthread_create(&inThreads[x], NULL, producer, (void *) &threaddata) != 0){
            fprintf(stderr, "error: Cannot create thread # %d\n", x);
            break;
        }
    }

    for( int y = 0; y < outThreadNumber; y++){
        threaddata.threadNumber=y;
        if( pthread_create(&outThreads[y], NULL, consumer, (void *) &threaddata) != 0){
            fprintf(stderr, "error: Cannot create thread # %d\n", y);
            break;
        }
    }


/*     for(int i = 0; i < inThreadNumber; i++){ */
/*         threaddata.threadNumber=i; */
/*              pthread_attr_t attr; */
/*              pthread_attr_init(&attr); */
/*         if( pthread_create(&inThreads[i], &attr, producer, (void *) &threaddata) != 0){ */
/*             fprintf(stderr, "error: Cannot create thread # %d\n", i); */
/*             break; */
/*         } */
/*         /1* pthread_create(&tid, &attr, producer, (void *) &threaddata); *1/ */
/*     } */

/*     for(int y = 0; y < outThreadNumber; y++){ */
/*         threaddata.threadNumber=y; */
/*              pthread_attr_t attr; */
/*              pthread_attr_init(&attr); */
/*         if( pthread_create(&outThreads[y], &attr, consumer, (void *) &threaddata) != 0){ */
/*             fprintf(stderr, "error: Cannot create thread # %d\n", y); */
/*             break; */
/*         } */
/*     } */

    close(tempinputFile);
    sleep(18);
    
    close(inputFile);
    close(outputFile);
    fclose(fp);

    return 0;
}
