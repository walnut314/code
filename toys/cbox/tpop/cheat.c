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

#define exchg(a, b) { int t = a; a = b; b = t; }
void sort(int a[], int n)
{
    for (int i = 1; i <= n-1; i++) {
        int d = i;
        while (d > 0 && a[d] < a[d-1]) { exchg(a[d], a[d-1]); d--; }
    }
}

void show(int a[], int n) {
    for (int i = 0; i < n; i++) { printf("%d) %d\n", i, a[i]); } printf("\n");
}

int main()
{
    int a[] = {2,6,4,9,1,3,2};
    printf("count bits: %d\n", count_bits(0x1713));
    show(a, 7);
    sort(a, 7);
    show(a, 7);

}
