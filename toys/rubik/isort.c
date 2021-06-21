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

void isort(int a[], int n)
{
    for (int i = 1; i < n; i++) {
        show(a, i);
        for (int j = i; j > 0; j--) { // sift up j
            if (a[j] < a[j-1]) SWAP(j, j-1);
        }
    }
}


void main()
{
    int n = sizeof(a)/sizeof(a[0]);
    show(a, n);
    isort(a, n);
    show(a, n);
}

