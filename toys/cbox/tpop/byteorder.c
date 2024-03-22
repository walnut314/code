#include <stdio.h>

int main()
{
    unsigned long x;
    unsigned char *p;
    int i;
    
    /*  11 22 33 44 => big-endian - high-byte high-address
       |0 |1 |2 |3 | address
       |11|22|33|44|

        44 33 22 11 => little-endian - high-byte low-address
       |0 |1 |2 |3 | address
       |44|33|22|11|

       x = 0x1122334455667788UL; for 64-bit long
    */

    // x = 0x11223344UL; // 32-bit long
    x = 0x1122334455667788UL; // 64-bit long
    p = (unsigned char *) &x;
    printf("ulong address %016lx\n", (unsigned long) p);
    for (i = 0; i < sizeof(long); p++, i++) {
        printf("%02lx ", ((unsigned long) p) & 0xff);
    }
    printf(" ==> address (LSBs)\n");
    p = (unsigned char *) &x;
    for (i = 0; i < sizeof(long); i++)
        printf("%02x ", *p++);
    printf(" ==> bytes\n");
    return 0;
}

