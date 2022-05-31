#include <stdio.h>

int main()
{
    unsigned long x;
    unsigned char *p;

    x = 0x11223344UL;
    p = (unsigned char *) &x;

    printf("x: (H) %x (L)\n", x);
    for (int i = 0; i < sizeof(long); i++) {
        printf(" %p -> %x\n", p, *p);
        p++;
    }
    printf("\n");
}
