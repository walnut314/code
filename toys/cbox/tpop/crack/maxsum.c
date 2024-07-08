#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// https://github.com/ericwang14/Programming-Pearls-source-code

float maxsum1(float x[], int n)
{
    float maxsum;
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            float sum = 0;
            for (int k = i; k < j; k++) {
                sum += x[k];
            }
            maxsum = fmax(maxsum, sum);
        }
    }
    return maxsum;
}

float maxsum2(float x[], int n)
{
    float maxsum;
    for (int i = 0; i < n; i++) {
        float sum = 0;
        for (int j = i; j < n; j++) {
            sum += x[j];
            maxsum = fmax(maxsum, sum);
        }
    }
    return maxsum;
}

float maxsum2b(float x[], int n)
{
    float *realarray = (float *) malloc(sizeof(float) * n+2);
    float *cumarr = &realarray[1];
    realarray[0] = 0;
    for (int i = 0; i < n; i++) {
        cumarr[i] = cumarr[i-1] + x[i];
    }
    float maxsofar = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            float sum = cumarr[j] - cumarr[i-1];
            maxsofar = fmax(maxsofar, sum);
        }
    }
    return maxsofar;
}

float maxsum3(float x[], int l, int u)
{
    float lmax, rmax, sum;    
    if (l > u) return 0.0;
    if (l == u) return fmax(0, x[l]);
    int m = (l+u)/2;
    lmax = sum = 0.0;
    for (int i = m; i >= l; i--) {
        sum += x[i];
        lmax = fmax(lmax, sum);
    }
    rmax = sum = 0.0;
    for (int i = m+1; i <= u; i++) {
        sum += x[i];
        rmax = fmax(rmax, sum);
    }
    return fmax(lmax+rmax, fmax(maxsum3(x,l,m), maxsum3(x,m+1,u)));
}

float maxsum4(float x[], int n)
{
    float maxsofar = 0.0, maxendinghere = 0.0;
    for (int i = 0; i < n; i++) {
        maxendinghere = fmax(maxendinghere + x[i], 0);
        maxsofar = fmax(maxsofar, maxendinghere);
    }
    return maxsofar;
}

int main()
{
    //int a[] = {'a','b','c','d','e','f','g','h'};
    float a[] = {31.0, -41.0, 59.0, 26.0, -53.0, 58.0, 97.0, -93.0, -23.0, 84.0};
    int len = sizeof(a) / sizeof(a[0]);
    //shift_left(a, 3, len);

    printf("maxsum1:  %.1f\n", maxsum1(a, len));
    printf("maxsum2:  %.1f\n", maxsum2(a, len));
    printf("maxsum2b: %.1f\n", maxsum2b(a, len));
    printf("maxsum3:  %.1f\n", maxsum3(a, 0, len-1));
    printf("maxsum4:  %.1f\n", maxsum4(a, len));
}

