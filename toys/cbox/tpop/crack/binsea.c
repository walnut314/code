#include <stdio.h>

// https://github.com/ericwang14/Programming-Pearls-source-code


int binarysearch(int *x, int n, int t)
{
    int l, u, m;
    l = 0; u = n-1;
    while (l <= u) {
        m = (l+u)/2;
        if (x[m] < t) l = m+1;
        else if (x[m] == t) return m;
        else u = m-1;
    }
    return -1;
}

void show(int *x, int n)
{
    for (int i = 0; i < n; i++) {
        printf("%d ", x[i]);
    }
    printf("\n");
}

#define MAXN (20)
int main()
{
    int x[MAXN];
    int t, n;
    while (scanf("%d %d", &n, &t)) {
        for (int i = 0; i < n; i++) {
            x[i] = 10 * i;
        }
        show(x, n);
        printf(" %d\n", binarysearch(x, n, t));
    }
            
}

