#include <stdio.h>

int quicky(int n)
{
    printf("quicky: 0x%x => 0n%d\n", n, n);
    return n;
}

int main()
{
    printf("hello, world\n");
    printf("quicky: %d\n", quicky(0xdeabeef));
}
