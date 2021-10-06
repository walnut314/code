#include <stdio.h>
#include <math.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define FIFO_LIMIT (16) // bytes

void split(int size)
{
    int n_trans = size / FIFO_LIMIT;
    int n_bytes = MIN(size, FIFO_LIMIT);
    int buffer_offset = 0;

    n_trans = (size % FIFO_LIMIT == 0) ? (size / FIFO_LIMIT) : (size / FIFO_LIMIT) + 1;

    printf("split[%d] => %d\n", size, n_trans);
    do {
        n_bytes = MIN(size, FIFO_LIMIT);
        printf("transmit[%02d] => %02d off: %02d\n", n_trans, n_bytes, buffer_offset);
        size -= n_bytes;
        buffer_offset += n_bytes;
    } while (--n_trans > 0);
}

int main()
{
    int size = 33;
    split(size);
}

