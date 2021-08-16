#include <stdio.h>

char gen_par(unsigned char byte)
{
    char parity;
    char bit;
    char pos;
    parity = 0;
    for (pos = 0; pos < 8; pos++) {
        bit = byte >> pos;
        bit &= 1;
        parity ^= bit;
    }
    printf("byte: %x - parity %x\n", byte, parity);
    return parity;
}

int main()
{
    int b = 0x5b;
    int p = gen_par(b);
    printf("par = %x\n", p);
}
