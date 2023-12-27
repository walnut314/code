#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void show(int a[], int n)
{
    printf("[%d,%d] ", 0, n);
    for (int i = 0; i < n; i++) {
        printf("%d, ", a[i]);
    }
    printf("\n");
}

void swap(int a[], int i, int j)
{
    int tmp = a[i]; a[i] = a[j]; a[j] = tmp;
}

void quicksort(int v[], int n)
{
    int i, last;
    if (n <= 1) return;
    swap(v, 0, rand() % n);
    for (last = 0, i = 1; i < n; i++) {
        if (v[i] < v[0])
            swap(v, ++last, i); // move m to proper location
    }
    swap(v, 0, last);
    quicksort(v, last);
    quicksort(v+last+1, n-last-1);
}

void main()
{
    int a[] = {55,41,59,26,53,58,97,93};
    int n = sizeof(a)/sizeof(a[0]);
    time_t t;

    srand((unsigned) time(&t));

    show(a, n);
    quicksort(a, n);
    show(a, n);
}

