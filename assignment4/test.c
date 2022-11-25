#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <string.h>

typedef struct circular_buffer{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t size;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} circular_buffer;

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

void cb_free(circular_buffer *circularbuffer){
    free(circularbuffer->buffer);
}

void cb_push_back(circular_buffer *circularbuffer, const void *item){
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

void cb_pop_front(circular_buffer *circularbuffer, void *item){
    if(circularbuffer->count == 0){
        printf("Buffer is empty");
    }
    memcpy(item, circularbuffer->tail, circularbuffer->size);
    circularbuffer->tail = (char*)circularbuffer->tail + circularbuffer->size;
    if(circularbuffer->tail == circularbuffer->buffer_end)
        circularbuffer->tail = circularbuffer->buffer;
    circularbuffer->count--;
}




int main(){
    char t = 'a';
    char t1 = 'b';
    char t2 = 'c';

    circular_buffer circularbuffer;
    circularBufferInit(&circularbuffer, 10, sizeof(char));
    cb_push_back(&circularbuffer, &t);
    // printf("head %c\n" , *(char*)circularbuffer.head);
    // printf("%c\n" , *(char*)circularbuffer.tail);
    printf("size %ld\n" , circularbuffer.count);
    cb_push_back(&circularbuffer, &t1);
    // printf("head %c\n" , *(char*)circularbuffer.head);
    // printf("%c\n" , *(char*)circularbuffer.tail);
    printf("size %ld\n" , circularbuffer.count);
    cb_push_back(&circularbuffer, &t2);
    // printf("head %c\n" , *(char*)circularbuffer.head);
    // printf("%c\n" , *(char*)circularbuffer.tail);
    printf("size %ld\n" , circularbuffer.count);

    printf("pop\n");

    cb_pop_front(&circularbuffer, &t);
    printf("size %ld\n" , circularbuffer.count);

    cb_pop_front(&circularbuffer, &t1);
    printf("size %ld\n" , circularbuffer.count);

    cb_pop_front(&circularbuffer, &t2);
    printf("size %ld\n" , circularbuffer.count);


   
}