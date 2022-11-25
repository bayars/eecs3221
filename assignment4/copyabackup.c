#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define TEN_MILLIS_IN_NANOS 10000000


typedef struct{
    char data;
    off_t offset ;
} Data ;


typedef struct{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t size;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
    char *sourceFilename;
    char *destinationFilename;
} circular_buffer;

struct timespec t;

//write to log file
void writeToLog(char *operation, char *threadType, int threadNumber, int offset,int b,int i){
    FILE *fp;
    fp = fopen("dataset4log.txt", "a");
    fprintf(fp, "%s %s%d 0%d B%d I%d\n", operation, threadType, threadNumber, offset, b, i); 
    fclose(fp);
}

void circularBufferInit(circular_buffer *circularbuffer, size_t capacity, size_t size){
    circularbuffer->buffer = malloc(capacity * size);
    if(circularbuffer->buffer == NULL)
        printf("Error in malloc");
    circularbuffer->buffer_end = (char *)circularbuffer->buffer + capacity * size;
    circularbuffer->capacity = capacity;
    circularbuffer->count = 0;
    circularbuffer->size = size;
    circularbuffer->head = circularbuffer->buffer;
    circularbuffer->tail = circularbuffer->buffer;
}

void circularBufferFree(circular_buffer *circularbuffer){
    free(circularbuffer->buffer);
}

void circularBufferpush_back(circular_buffer *circularbuffer, const void *item){
    if(circularbuffer->count == circularbuffer->capacity){
        // handle error
        printf("Buffer is full");
    }
    memcpy(circularbuffer->head, item, circularbuffer->size);
    circularbuffer->head = (char*)circularbuffer->head + circularbuffer->size;
    if(circularbuffer->head == circularbuffer->buffer_end)
        circularbuffer->head = circularbuffer->buffer;
    circularbuffer->count++;
}

void circularBufferPop_front(circular_buffer *circularbuffer, void *item){
    if(circularbuffer->count == 0){
        printf("Buffer is empty");
    }
    memcpy(item, circularbuffer->tail, circularbuffer->size);
    circularbuffer->tail = (char*)circularbuffer->tail + circularbuffer->size;
    if(circularbuffer->tail == circularbuffer->buffer_end)
        circularbuffer->tail = circularbuffer->buffer;
    circularbuffer->count--;
}

int p = 0;
void *producer(void *buffer){
    circular_buffer *circularbuffer = (circular_buffer *)buffer;
    Data data;
    int fd = open(circularbuffer->sourceFilename , O_RDONLY);
    if(fd == -1){
        printf("Error in opening file");
        exit(1);
    }
    while(1){
        if(read(fd, &data.data, 1) == 0){
            break;
        }
        data.offset = lseek(fd, 0, SEEK_CUR);
        circularBufferpush_back(circularbuffer, &data);
        printf("Producer: %c", data.data);
        writeToLog("read_byte", "PT", p, data.offset,data.data,-1);
        writeToLog("produce", "PT", p, data.offset,data.data,0);
        t.tv_sec = 0;
        t.tv_nsec = rand()%(TEN_MILLIS_IN_NANOS+1);
        nanosleep(&t, NULL);
        p++;
    }
    close(fd);
    return NULL;
}

int z = 0;
void *consumer(void *buffer){
    circular_buffer *circularbuffer = (circular_buffer *)buffer;
    Data data;
    int fd = open(circularbuffer->destinationFilename , O_WRONLY | O_CREAT, 0666);
    if(fd == -1){
        printf("Error in opening file");
        exit(1);
    }
    while(1){
        circularBufferPop_front(circularbuffer, &data);
        printf("Consumer: %c", data.data);
        if(data.data == '\n'){
            break;
        }
        lseek(fd, data.offset, SEEK_SET);
        write(fd, &data.data, 1);
        writeToLog("write_byte", "CT", z, data.offset,data.data,-1);
        writeToLog("consume", "CT", z, data.offset,data.data,0);
        t.tv_sec = 0;
        t.tv_nsec = rand()%(TEN_MILLIS_IN_NANOS+1);
        nanosleep(&t, NULL);
        z++;
    }
    close(fd);
    return NULL;
}



int main(int argc, char *argv[]) {
    Data copy_data;

    FILE *sourceFile;
    FILE *destinationFile;
    FILE *fplogFileName;

    int inThreadNumber = atoi(argv[1]);
    int outThreadNumber = atoi(argv[2]);
    char *fileName = argv[3];
    char *copyFileName = argv[4];
    int bufferSize = atoi(argv[5]);
    char *logFileName = argv[6];

    pthread_t inThread[inThreadNumber];
    pthread_t outThread[outThreadNumber];

    if( inThreadNumber < 1 ){
        printf("Error: Invalid number of input threads");
        exit(1);
    }

    if( outThreadNumber < 1 ){
        printf("Error: Invalid number of output threads");
        exit(1);
    }

    if( !(sourceFile = fopen(fileName, "r")) ){
        printf("Error: file does not exist");
        exit(1);
    }

    if( !(destinationFile = fopen(copyFileName, "w")) ){
        printf("Error: file does not exist");
        exit(1);
    }

    if( bufferSize < 1 ){
        printf("Error: Invalid buffer size");
        exit(1);
    }

    if( !(fplogFileName = fopen(logFileName, "r")) ){
        printf("Error: file does not exist");
        exit(1);
    }

    circular_buffer circularbuffer;
    circularbuffer.destinationFilename = copyFileName;
    circularbuffer.sourceFilename = fileName;
    circularBufferInit(&circularbuffer, bufferSize, sizeof(Data));
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, &circularbuffer);
    pthread_create(&consumer_thread, NULL, consumer, &circularbuffer);
    sleep(3);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    circularBufferFree(&circularbuffer);


    // fseek (sourceFile, 1, SEEK_SET);
    // copy_data[0].offset = ftell (sourceFile);
    // copy_data[0].data  = getc (sourceFile);

    // printf ("Character at position %d = '%c'.\n\n", copy_data[0].offset , copy_data[0].data );

    // if (fseek(destinationFile, copy_data[0].offset, SEEK_SET) == -1) {
    //     fprintf(stderr, "error setting output file position to %u\n",
    //     (unsigned int) copy_data[0].offset);
    //     exit(-1);
    // }
    // if (fputc(copy_data[0].data, destinationFile) == EOF) {
    //     fprintf(stderr, "error writing byte %d to output file\n", copy_data[0].data);
    //     exit(-1);
    // }


    // printf("inThreadNumber: %d\n", inThreadNumber);
    // printf("outThreadNumber: %d\n", outThreadNumber);
    // printf("fileName: %s\n", fileName);
    // printf("copyFileName: %s\n", copyFileName);
    // printf("bufferSize: %d\n", bufferSize);
    // printf("logFileName: %s\n", logFileName);


    // create buffer
    // BufferItem *buffer = malloc(sizeof(BufferItem) * buffer_size);
    // if (buffer == NULL) {
    //     printf("Error creating buffer");
    //     exit(1);
    // }

    // // create threads
    // pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    // if (threads == NULL) {
    //     printf("Error creating threads");
    //     exit(1);
    // }

    // // create mutex
    // pthread_mutex_t mutex;
    // if (pthread_mutex_init(&mutex, NULL) != 0) {
    //     printf("Error creating mutex");
    //     exit(1);
    // }

    // // create condition variables
    // pthread_cond_t empty;
    // if (pthread_cond_init(&empty, NULL) != 0) {
    //     printf("Error creating empty condition variable");
    //     exit(1);
    // }

    // pthread_cond_t full;
    // if (pthread_cond_init(&full, NULL) != 0) {
    //     printf("Error creating full condition variable");
    //     exit(1);
    // }

    // // create thread arguments
    // struct thread_args {
    //     int thread_id;
    //     int num_items;
    //     FILE *input;
    //     FILE *output;
    //     BufferItem *buffer;
    //     int buffer_size;
    //     pthread_mutex_t *mutex;
    //     pthread_cond_t *empty;
    //     pthread_cond_t *full;
    // };

    // struct thread_args *args = malloc(sizeof(struct thread_args) * num_threads);
    // if (args == NULL) {
    //     printf("Error creating thread arguments");
    //     exit(1);
    // }

    // // create threads

    // // join threads

    // // free memory


    // return 0;
}