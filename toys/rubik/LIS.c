#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

const int N = 9;
//int X[] = {-7, 10, 9, 2, 3, 8, 8, 1};
//int X[] = {50, 3, 10, 7, 40, 80};
int X[] = {10, 22, 9, 33, 21, 50, 41, 60, 80};
int O[9] = {0};

int LIS(int i)
{
#if 1
    // non-recursive
    int max = 1;
    O[0] = 1;
    for (int j = 1; j < N; j++) {
        if (X[j] > X[j-1]) {
            O[j] = 1 + O[j-1];
        } else {
            O[j] = O[j-1];
        }
        if (O[j] > max) max = O[j];
    }
    //return max; // O[N-1] is max
    return O[N-1];
#endif
#if 0
    // recursive
    int ans = 1;
    for (int j = 0; j < i; j++) {
        if (X[i] > X[j]) { // extend LIS(j) with i
            ans = max(ans, 1 + LIS(j));
        }
    }
    return ans;
#endif
}

int main()
{
    int max = 0;
//    for (int i = 0; i < N; i++) {
//        int ans = LIS(i);
//        if (ans > max) max = ans;
//    }
    max = LIS(N);
    printf("%d\n", max);
}
