#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <dispatch/dispatch.h>

#define FIFOSIZE    (10)
uint32_t PutI;
uint32_t GetI;
uint32_t Fifo[FIFOSIZE];
int32_t CurrentSize;
uint32_t LostData;

int32_t r1 = 1;
int32_t r2 = 2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar   = PTHREAD_COND_INITIALIZER;

sem_t sem;
dispatch_semaphore_t sema;
dispatch_semaphore_t *psema;

typedef struct {
    int buf[FIFOSIZE]; // the buffer acts like a stack, if needed implement a queue
    size_t len; // number of items in the buffer
    pthread_mutex_t mutex; // needed to add/remove data from the buffer
    pthread_cond_t can_produce; // signaled when items are removed
    pthread_cond_t can_consume; // signaled when items are added
} buffer_t;



void OS_Wait(int32_t *s)
{
    //pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condvar, &mutex);
    while ((*s) == 0) ;
    (*s) = (*s) - 1;
    //pthread_mutex_unlock(&mutex);
}

void OS_Signal(int32_t *s)
{
    pthread_mutex_lock(&mutex);
    (*s) = (*s) + 1;
    pthread_cond_signal(&condvar);
    pthread_mutex_unlock(&mutex);
}

void OS_FIFO_Init()
{
    memset(Fifo, 0, FIFOSIZE);
    PutI = GetI = 0;
    LostData = 0;
    CurrentSize = 0;
}

int OS_FIFO_Put(uint32_t data)
{
    if (CurrentSize == FIFOSIZE) {
        printf("lost data\n");
        LostData++;
        return -1; // full
    } else {
        printf("put data %d @ %d\n", data, PutI);
        Fifo[PutI+1] = data;
        PutI = (PutI+1) % FIFOSIZE;
        //OS_Signal(&CurrentSize);
        return 0;
    }
}

uint32_t OS_FIFO_Get()
{
    //OS_Wait(&CurrentSize); // block if empty
    uint32_t data = Fifo[GetI];
    GetI = (GetI+1) % FIFOSIZE;
    return data;
}

void *Produce(void *arg)
{
    buffer_t *buffer = (buffer_t*)arg;
    uint32_t data = 1;
    while (1) {
        pthread_mutex_lock(&mutex);
        OS_FIFO_Put(data++);
        CurrentSize++;
        printf("suceessful put: %d sz: %d\n", data-1, CurrentSize);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condvar);
        printf("sleeping\n");
        sleep(1);
 
     }
    return NULL;
}

void *Consume(void *arg)
{
    buffer_t *buffer = (buffer_t*)arg;
    while (1) {
        pthread_cond_wait(&condvar, &mutex);
        pthread_mutex_lock(&mutex);
        uint32_t data = OS_FIFO_Get();
        printf("suceessful get: %d\n", data);
        pthread_mutex_unlock(&mutex);
     }
    return NULL;
}

void *Prod(void *arg)
{
    while (1) {
        printf("dude, post a sem\n");
        //sem_post(&sem);
        sleep(3);
        dispatch_semaphore_signal(sema);
        
        //pthread_mutex_unlock(&mutex);
        //printf("prod got it\n");
        //pthread_mutex_lock(&mutex);
        sleep(3);
        //pthread_cond_signal(&condvar);
    }
    return NULL;
}

void *Cons1(void *arg)
{
    while (1) {
        //pthread_cond_wait(&condvar, &mutex);
        printf("wait1\n");
        sleep(6);
        dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);        
        printf("cons1 got a sem\n");
        //pthread_mutex_lock(&mutex);
        //printf("cons1 got one\n");
        //pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    return NULL;
}

void *Cons2(void *arg)
{
    while (1) {
        //pthread_cond_wait(&condvar, &mutex);
        //pthread_mutex_lock(&mutex);
        //printf("cons2 got one\n");
        //pthread_mutex_unlock(&mutex);
        printf("cons2\n");
        sleep(2);
    }
    return NULL;
}

void *Cons3(void *arg)
{
    while (1) {
        //pthread_cond_wait(&condvar, &mutex);
        //pthread_mutex_lock(&mutex);
        //printf("cons3 got one\n");
        //pthread_mutex_unlock(&mutex);
        printf("cons3\n");
        sleep(2);
    }
    return NULL;
}


int main()
{
    buffer_t buffer = {
        .len = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .can_produce = PTHREAD_COND_INITIALIZER,
        .can_consume = PTHREAD_COND_INITIALIZER
    };

#if 0
    psema = &sema;
    //psema = dispatch_semaphore_create(0);

    //sem_init(&sem, 0, 0);

    pthread_t prod, cons1, cons2, cons3;
    pthread_create(&prod, NULL, Prod, (void *) &buffer);
    pthread_create(&cons1, NULL, Cons1, (void *) &buffer);
    //pthread_create(&cons2, NULL, Cons2, (void *) &buffer);
    //pthread_create(&cons3, NULL, Cons3, (void *) &buffer);

    pthread_join(prod, NULL);
    pthread_join(cons1, NULL);
    //pthread_join(cons2, NULL);
    //pthread_join(cons3, NULL);
#endif
#if 1
    //Produce(&CurrentSize);
    //Consume(&CurrentSize);
    OS_FIFO_Init();
    pthread_t prod, cons;

    pthread_create(&prod, NULL, Produce, (void *) &buffer);
    pthread_create(&cons, NULL, Consume, (void *) &buffer);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    //do_wrap_up(r1, r2);
#endif

}


