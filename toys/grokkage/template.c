#include <stdio.h>
#include <stdint.h>

// http://blog.pkh.me/p/20-templating-in-c.html

#define INT8     (8)
#define INT16   (16)
#define INT32   (32)


#define DO_RANDOM_STUFF(type) do {      \
    int i;                              \
    type *p = buf;                      \
                                        \
    for (i = 0; i < len; i++)           \
        p[i] = p[i] * k;                \
} while (0)

int func(void *buf, int len, float k, int request)
{
    if      (request == INT8)   DO_RANDOM_STUFF(int8_t);
    else if (request == INT16)  DO_RANDOM_STUFF(int16_t);
    else if (request == INT32)  DO_RANDOM_STUFF(int32_t);
}

int main()
{
    int8_t  b8[4];
    int16_t b16[4];
    int32_t b32[4];

    func((void *) b8, 4, 1.3, INT8);
    for (int i = 0; i < 4; i++) {
        printf("[%d] = %d\n", i, b8[i]);
    }
}

