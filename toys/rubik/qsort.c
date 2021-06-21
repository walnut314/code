#include <stdio.h>

int a[] = {3,1,4,2};

#define SWAP(i, j) { int tmp = a[i]; a[i] = a[j]; a[j] = tmp; }

void show(int a[], int n)
{
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%d, ", a[i]);
    }
    printf("\n");
}

void partition(int a[], int l, int u)
{
    int m = (l+u)/2;
    int p = a[l];
    SWAP(l, m);
    for (int i = l; i < u; i++) {
        if (a[l] > a[m]) {
            SWAP(l, u);
        }
    }
}

void qsort(int a[], int l, int u)
{
    if (l >= u) return;
    int p = l;
    partition(a, l, u);
    show(a, u);
    
    for (int i = l; i < u; i++) {
        for (int j = i; j > 0; j--) { // sift up j
            if (a[j] < a[j-1]) SWAP(j, j-1);
        }
    }
}


void main()
{
    int n = sizeof(a)/sizeof(a[0]);
    show(a, n);
    qsort(a, 0, n);
    show(a, n);
}

