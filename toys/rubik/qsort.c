#include <stdio.h>

int a[] = {55,41,59,26,53,58,97,93};

#define SWAP(i, j) { int tmp = a[i]; a[i] = a[j]; a[j] = tmp; }

void show(int a[], int n)
{
    printf("[%d,%d] ", 0, n);
    for (int i = 0; i < n; i++) {
        printf("%d, ", a[i]);
    }
    printf("\n");
}

void swap(int i, int j)
{
    int tmp = a[i]; a[i] = a[j]; a[j] = tmp;
}

void qsort(int a[], int l, int u)
{
    if (l >= u) return;
    int m = l;
    for (int i = l+1; i <= u; i++) {
        if (a[i] < a[l])
            swap(++m, i); // move m to proper location
    }
    swap(l, m); // then swap l and m
    printf("> "); show(a, u);
    qsort(a, l, m-1);
    qsort(a, m+1, u);
}

// sedgewick
void qsort2(int a[], int l, int r)
{
    int v, i, j, t;
    if (r > l) {
        v = a[r]; i = l-1; j = r;
        for (;;) {
            while (a[++i] < v) ; // increment i to v
            while (a[--j] > v) ; // decrement j to v
            if (i >= j) break; // after this i and j are bounding v
            t = a[i]; a[i] = a[j]; a[j] = t;
        }
        t = a[i]; a[i] = a[r]; a[r] = t;
        qsort2(a, l, i-1);
        qsort2(a, i+1, r);
    }
}

void main()
{
    int n = sizeof(a)/sizeof(a[0]);
    show(a, n);
    qsort(a, 0, n);
    show(a, n);
}

