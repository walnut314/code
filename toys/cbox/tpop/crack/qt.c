#include <stdio.h>

int count_bits(int val)
{
    int count = 0;
    while (val) {
        count++;
        val &= (val-1);
    }
    return count;
}




int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    int val = 0x3013;
    printf("there are %d bits in %x\n", count_bits(val), val);
}
