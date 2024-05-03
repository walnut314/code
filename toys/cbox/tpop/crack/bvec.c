#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define N_BITS   (8*1024*1024)
#define N_BYTES  (N_BITS/8)

typedef struct _BVEC {
    uint64_t n_bits;
    char bits[N_BYTES];
} BVEC;

void setbit(BVEC *vec, int bit)
{
}

int getbiit(BVEC *vec, int bit)
{
    return 0;
}

int main()
{
    BVEC vec = {0};
    vec.n_bits = N_BITS;

    printf("bvec bytes: %dK\n", N_BYTES/1024);

}

