#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void swap(int v[], int i, int j)
{
    int temp;
    temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

void quicksort(int v[], int n)
{
    int i, last;
    if (n <= 1) return;
    swap(v, 0, rand() % n);
    for (last = 0, i = 1; i < n; i++) {
        if (v[i] < v[0]) swap(v, ++last, i);
    }
    swap(v, 0, last);
    quicksort(v, last);
    quicksort(v+last+1, n-last-1);
}

void show(int v[], int n)
{
    for (int i = 0; i < n; i++) {
        printf("[%d] => %d\n", i, v[i]);
    }
    printf("\n");
}


int main()
{
    time_t t;
    int a[] = {4,2,8,5,9};
    int n = sizeof(a)/sizeof(a[0]);

    srand((unsigned) time(&t));

    show(a, n);
    quicksort(a, n);
    show(a, n);
}

