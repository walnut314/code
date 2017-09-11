#include <stdio.h>

/* Function to reverse bits of num */
unsigned int reverseBits(unsigned int num)
{
    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int reverse_num = 0, i, temp;
 
    printf("n %d\n", NO_OF_BITS);
    printf("reversing: %08x\n", num);
    for (i = 0; i < NO_OF_BITS; i++)
    {
        temp = (num & (1 << i));
        if(temp)
            reverse_num |= (1 << ((NO_OF_BITS - 1) - i));
    }
  
    printf("reversed: %08x\n", reverse_num);
    return reverse_num;
}

// also used to count bits
int parity(unsigned int num)
{
    int i;
    int parity = 0;
    while (num) {
        num &= (num-1);
        parity ^= 1;
    }
    return parity;
}

int main()
{
    int i;
    reverseBits(0x0000005a);
    for (i = 0; i < 8; i++) {
        printf("parity of %d is %d\n", i, parity(i));
    }
}
