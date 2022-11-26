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
    Data *data;
    int threadNumber;
    int bufferSize;
    Data buffer[];
}ThreadData;


struct timespec t;
Data buffer[];
int insertPointer=0;
int removePointer=0;
// int *bufSize=51;


pthread_mutex_t mutex;
sem_t empty;
sem_t full;


int inputFile;
int outputFile;
int logFile;
FILE *fp;


//write to log file
void writeToLog(char *operation,int threadNumber, Data *bufferItem, int i){
    fprintf(fp, "%s P%d 0%ld B%d I%d\n", operation, threadNumber, bufferItem->offset, bufferItem->data, i); 
}

void nsleep(){
    t.tv_sec = 0;
    t.tv_nsec =  rand() % TEN_MILLIS_IN_NANOS;
    nanosleep(&t, NULL);
}

void read_byte(int threadNumber, Data *item){
    pthread_mutex_lock(&mutex);
    if((item->offset = lseek(inputFile,0,SEEK_CUR)) < 0){
        pthread_mutex_unlock(&mutex);
        fprintf(stderr, "Error: lseek failed1\n");
        exit(1);
    }
    printf("read_byte: offset = %ld, read data = %c \n", item->offset, item->data);
    if( read(inputFile, &item->data, 1) < 0){
        printf("read_byte PT%ld: EOF pthread_exit(0)\n", pthread_self());
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
    printf("data = %c \n", item->data);
    writeToLog("read_byte",threadNumber, item, -1);
    pthread_mutex_unlock(&mutex);
}

void write_byte(int threadNumber, Data *item){
    pthread_mutex_lock(&mutex);
    // printf("write_byte: %c\n", item->data);
    if(write(outputFile, &item->data, 1) < 1){
        pthread_mutex_unlock(&mutex);
        fprintf(stderr, "Error: write failed\n");
        exit(1);
    }
    writeToLog("write_byte",threadNumber, item, -1);
    pthread_mutex_unlock(&mutex);
}

void produce(int threadNumber, Data *item, int bufSize){
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    printf("thread %d: consume %c\n", threadNumber, item->data);
    buffer[insertPointer] = *item;
    // writeToLog("produce",threadNumber, item, insertPointer);
    insertPointer = (insertPointer+1) % bufSize;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
}

void consume(int threadNumber, Data *item, int bufSize){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    item =  &buffer[removePointer];
    // writeToLog("consume", threadNumber, item, removePointer);
    removePointer = (removePointer + 1) % bufSize;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty); 
}

void *producer( void * param){
    ThreadData *threadData = (ThreadData *) param;
    while (1){
        nsleep();
        read_byte(threadData->threadNumber, threadData->data);
        nsleep();
        produce(threadData->threadNumber, threadData->data, threadData->bufferSize);
    }
}

void *consumer( void * param){
    ThreadData *threadData = (ThreadData *) param;
    while (1){
        if(inputFile == EOF){
            pthread_exit(0);
            break;
        }
        // printf("Consumer nsleep() = %d\n", nsleep());
        nsleep();
        consume(threadData->threadNumber, threadData->data, threadData->bufferSize);
        nsleep();
        write_byte(threadData->threadNumber, threadData->data);
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

    sem_init(&empty, 0, bufferSize);
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);

    if( inThreadNumber < 1 ){
        printf("Error: Invalid number of input threads");
        exit(1);
    }

    if( outThreadNumber < 1 ){
        printf("Error: Invalid number of output threads");
        exit(1);
    }

    if( !(fopen(inputFileName, "r")) ){
        printf("Error: file does not exist");
        exit(1);
    }

    if( !(fopen(outputFileName, "w")) ){
        printf("Error: file does not exist");
        exit(1);
    }

    if( bufferSize < 1 ){
        printf("Error: Invalid buffer size");
        exit(1);
    }

    if( !(fopen(logFileName, "r")) ){
        printf("Error: file does not exist");
        exit(1);
    }
    // char buffer[bufferSize];

    outputFile = open("copied.txt", O_WRONLY | O_CREAT, 0666);
    inputFile = open("dataset4.txt", O_RDONLY,0666);
    fp = fopen("dataset4log.txt", "w");

    threaddata.bufferSize=bufferSize;
    threaddata.buffer[bufferSize];

    // threaddata.data.data = 'a';
    for(int i = 0; i < inThreadNumber; i++){
        threaddata.threadNumber=i;
        pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
        pthread_create(&tid, &attr, producer, (void *) &threaddata);
    }

    for(int y = 0; y < outThreadNumber; y++){
        threaddata.threadNumber=y;
        pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
        pthread_create(&tid, &attr, consumer, (void *) &threaddata);    
    }

    sleep(3);
    
    close(inputFile);
    close(outputFile);
    fclose(fp);

    return 0;
}