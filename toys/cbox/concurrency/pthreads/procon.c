#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUF_SIZE 5

typedef struct {
    int buf[BUF_SIZE]; // the buffer acts like a stack, if needed implement a queue
    size_t len; // number of items in the buffer
    pthread_mutex_t mutex; // needed to add/remove data from the buffer
    pthread_cond_t can_produce; // signaled when items are removed
    pthread_cond_t can_consume; // signaled when items are added
} buffer_t;

void* producer(void *arg) { // produce random numbers
    buffer_t *buffer = (buffer_t*)arg;
    while(1) {
        pthread_mutex_lock(&buffer->mutex);
        if(buffer->len == BUF_SIZE) { // full
            // wait until some elements are consumed
            pthread_cond_wait(&buffer->can_produce, &buffer->mutex);
        }
        int t = rand();
        printf("%p Produced: %d\n", pthread_self(), t);
        // append data to the buffer
        buffer->buf[buffer->len] = t;
        ++buffer->len;
        // signal the fact that new items may be consumed
        pthread_cond_signal(&buffer->can_consume);
        pthread_mutex_unlock(&buffer->mutex);
    }
    return NULL; // never reached
}

void* consumer(void *arg) { // consume random numbers
    buffer_t *buffer = (buffer_t*)arg;
    while(1) {
        pthread_mutex_lock(&buffer->mutex);
        // single consume -> use if
        //if(buffer->len == 0) { // empty
        // multiple consumers -> use while
        while(buffer->len == 0) { // while empty, wait for new items to be appended to the buffer
            pthread_cond_wait(&buffer->can_consume, &buffer->mutex);
        }
        --buffer->len; // grab data
        printf("%p Consumed: %d\n", pthread_self(), buffer->buf[buffer->len]);
        // signal the fact that new items may be produced
        pthread_cond_signal(&buffer->can_produce);
        pthread_mutex_unlock(&buffer->mutex);
    }
    return NULL; // never reached
}

int main(int argc, char *argv[]) {
    buffer_t buffer = {
        .len = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .can_produce = PTHREAD_COND_INITIALIZER,
        .can_consume = PTHREAD_COND_INITIALIZER
    };

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, (void*)&buffer);
    pthread_create(&cons, NULL, consumer, (void*)&buffer);

    pthread_join(prod, NULL); // will wait forever
    pthread_join(cons, NULL);

    return 0;
}

