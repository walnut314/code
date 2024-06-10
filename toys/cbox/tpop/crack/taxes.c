#include <stdio.h>

int interval_search(int *s, int n, int k)
{
/*
 * binary search of array s of size n
 * for the value k.
 */
    int low, high, mid, found = 0;
    low = 0;
    high = n-1;
    while (low <= high) {
        //printf("interval: low %d, hi %d, target %d\n", s[low], s[high], k);
        mid = (low+high)/2;
        //printf("is it between %d @ %d\n", s[mid], s[mid-1]);
        //printf("or is it between %d @ %d\n", s[mid], s[mid+1]);
        if (k > s[mid]) low = mid + 1;
        else if (k < s[mid]) high = mid - 1;
        else {
            found=1;
            break;
        }
    }
#if 0
    if (found) {
        printf("found mid: %d\n", mid);
    } else {
        printf("not found mid: %d\n", mid);
    }
#endif
    return mid;
}

float tax[] = {0.0,0.14,0.15,0.16,0.17,0.18,0.19,0.20};

int main()
{
    int bracket[] = {0, 2200, 2700, 3200, 3700, 4200, 4700, 10000};
    int len = sizeof(bracket)/sizeof(bracket[0]);
    int test[] = {-100, -1, 0, 1, 1099, 1100, 1101, 
                   2199, 2200, 2201, 
                   2399, 2400, 2401, 2699, 2700, 2701, 
                   2899, 2900, 2901, 3199, 3200, 3201, 
                   3399, 3400, 3401, 3699, 3700, 3701, 
                   3999, 4000, 4001, 4199, 4200, 4201, 
                   4399, 4400, 4401, 4699, 4700, 4701, 
                   8999, 9000, 9001, 9999, 10000, 10001};
    int tests = sizeof(test)/sizeof(test[0]);
    int ret; 

    for (int i = 0; i < tests; i++) {
        ret = interval_search(bracket, len, test[i]);
        printf("tax: %6d : mid [%d] => bracket %6d --> tax: %0.2f\n", test[i], ret, bracket[ret], tax[ret]);
    }

}
