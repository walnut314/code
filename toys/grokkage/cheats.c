#include <stdio.h>
#include <stdlib.h>

typedef struct _List {
    struct _List *next;
    int key;
} List;

void list_insert(List *head, int item)
{
}

void list_show(List *head)
{
}

int count_bits(int k)
{
    int cnt = 0;
    while (k) {
        cnt++;
        k &= (k - 1);
    }
    return cnt;
}

void insort2(int a[], int n)
{
    int i, j;
    for (j = 1; j < n; j++) {
        int key = a[j];
        for (i = j-1; i >= 0 && a[i] > key; i--) {
            a[i+1] = a[i];
        }
        a[i+1] = key;
    }
}

void insort(int a[], int n)
{
    int i, j;
    for (i = 1; i < n; i++) {
        int t = a[i];
        for (j = i; j > 0 && a[j-1] > t; j--) {
            a[j] = a[j-1];
        }
        a[j] = t;
    }
}

void array_show(int a[], int len)
{
    for (int i = 0; i < len; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main()
{
    printf("bits: %d\n", count_bits(0x17));

    int a[] = {5,2,4,6,1,3};
    int len = sizeof(a)/sizeof(a[0]);
    array_show(a, len);
    insort(a, len);
    array_show(a, len);

}
