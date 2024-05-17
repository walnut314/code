#include <stdio.h>

// https://github.com/ericwang14/Programming-Pearls-source-code


void reverse(int a[], int i, int j)
{
    while (i < j) {
        // swap 'em
        int t = a[i]; a[i] = a[j]; a[j] = t;
        i++; j--;
    }
}

void show(int a[], int n)
{
    printf("==> ");
    for (int i = 0; i < n; i++) {
        printf("%c, ", a[i]);
    }
    printf("\n");
}

void shift_left(int a[], int i, int n)
{
    reverse(a, 0, i-1); show(a, n);
    reverse(a, i, n-1); show(a, n);
    reverse(a, 0, n-1); show(a, n);
}

int main()
{
    int a[] = {'a','b','c','d','e','f','g','h'};
    int len = sizeof(a) / sizeof(a[0]);
    shift_left(a, 3, len);
}

