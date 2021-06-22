#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX_LEN 1000000
int a[MAX_LEN];

void swap(int i, int j)
{
    int tmp = a[i]; a[i] = a[j]; a[j] = tmp;
}

void show(int a[], int n)
{
    printf("[%d,%d] ", 0, n);
    for (int i = 0; i < n; i++) {
        printf("%d, ", a[i]);
    }
    printf("\n");
}

int compare (const void * num1, const void * num2)
{
    if (*(int*)num1 > *(int*)num2) return 1;
    return -1;
}

void generate()
{
    int n = sizeof(a)/sizeof(a[0]);
    for (int i = 0; i < n; i++)
        a[i] = i;
}

int randint(int l, int u)
{
    int r;
    while (1) {
        r = rand();
        if (r < u && r >= 0) break;
    }
    return r % u;
}

void shuffle()
{
    int n = sizeof(a)/sizeof(a[0]);
    for (int i = 0; i < n; i++) {
        swap(i, (randint(i, n-1)));
    }
}

void main()
{
    time_t t;
    srand((unsigned) time(&t));    
    int n = sizeof(a)/sizeof(a[0]);

    generate();
    //show(a, n);
    shuffle();
    //show(a, n);

    int m = 4;
    qsort(a, m, sizeof(int), compare);
    printf("final answer\n");
    show(a, m);
}

