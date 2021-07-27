#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "ring.h"

static int r_mask(ring_ptr r, uint32_t index);

struct ring {
    uint32_t capacity;
    uint32_t read;
    uint32_t write;
    void **arr;
} ring;

ring_ptr ring_create(int size)
{
    ring_ptr r = (ring_ptr) malloc(sizeof(*r));
    r->capacity = size;
    r->write = r->read = 0;
    r->arr = (void **) malloc(size * sizeof(void *));
    return r;
}

void ring_destroy(ring_ptr r)
{
    free(r->arr);
    free(r);
}

int ring_size(ring_ptr r)
{
    return (r->write - r->read);
}

int ring_empty(ring_ptr r)
{
    return (r->read == r->write);
}

int ring_full(ring_ptr r)
{
    return (ring_size(r) == r->capacity);
}

void ring_push(ring_ptr r, void *val)
{
    assert(!ring_full(r)); r->arr[r_mask(r, r->write++)] = val; 
}

void *ring_shift(ring_ptr r)
{
    assert(!ring_empty(r)); return r->arr[r_mask(r, r->read++)];
}

void ring_dump(ring_ptr r)
{
    int i;
    for (i = 0; i < r->capacity; i++) {
        printf("a[%02d] = %08llx\n", i, (uint64_t) r->arr[i]);
    }
}

static int r_mask(ring_ptr r, uint32_t index)
{
    return (index & (r->capacity - 1));
}

#if 1
void ring_test()
{
    int i;
    ring_ptr r1 = ring_create(8);
    ring_ptr r2 = ring_create(16);

    for (i = 0; i < 32; i++) {
        if (!ring_full(r1)) {
            ring_push(r1, (void *) i);
        }
        if (!ring_full(r2)) {
            ring_push(r2, (void *) i);
        }
    }

    printf("full : %d\n", ring_full(r1));
    printf("empty: %d\n", ring_empty(r1));
    printf("size : %d\n", ring_size(r1));

    printf("full : %d\n", ring_full(r2));
    printf("empty: %d\n", ring_empty(r2));
    printf("size : %d\n", ring_size(r2));
    
    while (!ring_empty(r1)) {
        printf("shift1: %d\n", ring_shift(r1));
    }

    while (!ring_empty(r2)) {
        printf("shift2: %d\n", ring_shift(r2));
    }

    ring_dump(r1);
    ring_dump(r2);
}
#endif

