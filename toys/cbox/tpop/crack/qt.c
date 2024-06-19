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

void isort(int *a, int n)
{
    for (int c = 1; c <= n-1; c++) {
        int d = c;
        while (d > 0 && a[d] < a[d-1]) {
            //exchg(a[d], a[d-1]);
            int temp = a[d];
            a[d] = a[d-1];
            a[d-1] = temp;
            d--;
        }
    }
}

void show(int *a, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    int val = 0x3013;
    printf("there are %d bits in %x\n", count_bits(val), val);

    int a[] = {3,2,5,1,9,6};
    int len = sizeof(a)/sizeof(a[0]);
    show(a, len);
    isort(a, len);
    show(a, len);
}
