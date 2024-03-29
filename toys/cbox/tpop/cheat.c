#include <stdio.h>

int count_bits(int x)
{
    int count = 0;
    while (x) {
        x &= (x-1);
        count++;
    }
    return count;
}


int main()
{
    printf("count bits: %d\n", count_bits(0x1713));
}
