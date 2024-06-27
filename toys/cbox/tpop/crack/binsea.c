#include <stdio.h>
#include <assert.h>
#include <time.h>

// https://github.com/ericwang14/Programming-Pearls-source-code

int sorted(int *x, int n)
{
    for (int i = 0; i < n-1; i++) {
        if (x[i] > x[i+1]) return 0;
    }
    return 1;
}

int binarysearch1(int *x, int n, int t) {
    int l=0, u=n-1, m, p = -1;
    assert(sorted(x, n));
    while (l <= u) {
        m = (l+u)/2;
        if (x[m] < t) l = m+1;
        else if (x[m] == t) {p = m; break;}
        else u = m-1;
    }
    return p;
}

int binarysearch2(int *x, int n, int t) {
    int l=0, u=n-1, m, p = -1;
    while (l <= u) {
        m = (l+u)/2;
        if (x[m] < t) l = m+1;
        else if (x[m] == t) {p = m; break;}
        else u = m-1;
    }
    return p;
}

void show(int *x, int n)
{
    printf("n %d: ", n);
    for (int i = 0; i < n; i++) {
        printf("%d ", x[i]);
    }
    printf("\n");
}

#define MAXN (20)
#define s1 binarysearch1
#define s2 binarysearch2

int main()
{
    clock_t begin, end;
    double time_spent;
     
    int x[MAXN];
    int t, n;
    //while (scanf("%d %d", &n, &t)) {
    begin = clock();

    for (int algnum = 0; algnum < 2; algnum++) {
    for (int iters = 0; iters < 99999; iters++) {
        //printf("iter: %d\n", iters);
        for (int j = 0; j < MAXN; j++) {
            for (int i = 0; i < j; i++) {
                x[i] = 10 * i;
            }
            //show(x, j);
            for (int i = 0; i < j; i++) {
                switch (algnum) {
                    case 0:
                        assert(s1(x, j, 10*i) == i);
                        assert(s1(x, j, 10*i-5) == -1);
                        break;
                    case 1:
                        assert(s2(x, j, 10*i) == i);
                        assert(s2(x, j, 10*i-5) == -1);
                        break;
                }
            }
            switch (algnum) {
                case 0:
                    assert(s1(x, j, 10*j-5) == -1);
                    assert(s1(x, j, 10*j) == -1);
                    break;
                case 1:
                    assert(s2(x, j, 10*j-5) == -1);
                    assert(s2(x, j, 10*j) == -1);
            }
            //printf(" %d\n", binarysearch(x, n, t));
        }
    }
    //}
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("algnum: %d end...time=%.2lf\n", algnum, time_spent);
    }
}

