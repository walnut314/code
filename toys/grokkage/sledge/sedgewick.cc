#include <stdio.h>
#include <string.h>
#include <math.h>

// sedgewick

#define MAXN 100

void arr_show(int a[], int start, int end)
{
    int i;
    for (i = start; i < end; i++) printf("%c ", a[i]);
    printf("\n");
}

void swap(int a[], int i, int j)
{
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

void bubsort(int a[], int n)
{
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = i; j < n; j++) {
            if (a[i] > a[j]) {
                swap(a, i, j);
            }
        }
    }
}

void selsort(int a[], int n)
{
    int i, j, min;
    for (i = 0; i < n; i++) {
        min = i;
        for (j = i+1; j < n; j++) {
            if (a[j] < a[min]) min = j;
        }
        swap(a, i, min);
    }
}

void inssort(int a[], int n)
{
    int i, j, v;
    for (i = 0; i < n; i++) {
        v = a[i]; j = i;
        while (a[j-1] > v) {
            a[j] = a[j-1]; j--;
        }
        a[j] = v;
    }
}

void indirect_sort(int a[], int p[], int n)
{
    int i, j, v;
    for (i = 0; i < n; i++) p[i] = i;
    for (i = 1; i < n; i++) {
        v = p[i]; j = i;
        while (a[p[j-1]] > a[v]) {
            p[j] = p[j-1];
            j--;
        }
        p[j] = v;
    }
}

unsigned int bits(unsigned int x, int k, int j)
{
    return ((x >> k) & ~(~0 << j));
}

void radix_exchange_sort(int a[], int l, int r, int b)
{
    int t, i, j;
    printf("radix: l:%d r:%d b:%d\n", l, r, b);
    if (r > l && b >= 0) {
    //if (r > l && b > 0) {
        i = l; j = r;
        while (j != i) {
            while (bits(a[i], b, l) == 0 && i < j) i++;
            while (bits(a[j], b, l) != 0 && j > i) j--;
            //printf("exchg: %d %d\n", i, j);
            t = a[i]; a[i] = a[j]; a[j] = t;
        }
        if (bits(a[r], b, l) == 0) j++;
        radix_exchange_sort(a, l, j-1, b-1);
        radix_exchange_sort(a, j, r, b-1);
    }
}

int partition(int a[], int l, int r)
{
    int i, j, v, t;
    v = a[r];
    i = l-1;
    j = r;
    for (;;) {
        while (a[++i] < v) ;
        while (a[--j] > v) ;
        if (i >= j) break;
        t = a[i]; a[i] = a[j]; a[j] = t;
    }
    t = a[i]; a[i] = a[r]; a[r] = t;
    return i;
}

void select_kth_largest(int a[], int l, int r, int k)
{
    int i;
    if (r > l) {
        i = partition(a, l, r);
        if (i > l+k-i) select_kth_largest(a, l, i-1, k);
        if (i < l+k-i) select_kth_largest(a, i+1, r, k-i);
    }
}

void append(int tape[], int i, int buf[], int j, int n)
{
    int it, ib;
    for (it = i, ib = j; ib < n; ib++, it++) {
        tape[it] = buf[ib];
    }
}



