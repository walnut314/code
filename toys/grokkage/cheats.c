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

void insort(int a[], int n)
{
    for (int i = 1; i <= n-1; i++) {
        int j = i;
        while (j > 0 && a[j] < a[j-1]) {
            int t = a[j];
            a[j] = a[j-1];
            a[j-1] = t;
            j--;
        }
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

    int a[] = {3,6,1,8,4};
    int len = sizeof(a)/sizeof(a[0]);
    array_show(a, len);
    insort(a, len);
    array_show(a, len);

}
