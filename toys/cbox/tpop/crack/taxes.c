#include <stdio.h>
#include "harness.h"

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
int bracket[] = {0, 2200, 2700, 3200, 3700, 4200, 4700, 10000};
int len = sizeof(bracket)/sizeof(bracket[0]);

int main()
{
    HARNESS htests[] = {
        -100, 0, 0, false,
        -1, 0, 0, false,
        0, 0, 0, false,
        1, 0, 0, false,
       1099, 0, 0, false,
       1100, 0, 0, false,
       1101, 0, 0, false,
       2199, 0, 0, false,
       2200, 0, 0, false,
       2201, 0, 1, false,
       2399, 0, 1, false,
       2400, 0, 1, false,
       2401, 0, 1, false,
       2699, 0, 1, false,
       2700, 0, 1, false,
       2701, 0, 2, false,
       2899, 0, 2, false,
       2900, 0, 2, false,
       2901, 0, 2, false,
       3199, 0, 2, false,
       3200, 0, 2, false,
       3201, 0, 3, false,
       3399, 0, 3, false,
       3400, 0, 3, false,
       3401, 0, 3, false,
       3699, 0, 3, false,
       3700, 0, 3, false,
       3701, 0, 4, false,
       3999, 0, 4, false,
       4000, 0, 4, false,
       4001, 0, 4, false,
       4199, 0, 4, false,
       4200, 0, 4, false,
       4201, 0, 5, false,
       4399, 0, 5, false,
       4400, 0, 5, false,
       4401, 0, 5, false,
       4699, 0, 5, false,
       4700, 0, 5, false,
       4701, 0, 6, false,
       8999, 0, 6, false,
       9000, 0, 6, false,
       9001, 0, 6, false,
       9999, 0, 6, false,
      10000, 0, 6, false,
      10001, 0, 6, false,
    };

    int tests = sizeof(htests)/sizeof(htests[0]);
    int ret; 

    for (int i = 0; i < tests; i++) {
        htests[i].out = interval_search(bracket, len, htests[i].in);
        if (htests[i].out == htests[i].expect) {
            htests[i].pass = true;
            printf("htests[%02d] = PASS\n", i);
        } else {
            printf("htests[%02d] = FAIL ==> ", i);
            printf("  expected: %d, rec'd: %d\n", htests[i].expect, htests[i].out);
        }
        //printf("tax: %6d : mid [%d] => bracket %6d --> tax: %0.2f\n", test[i], ret, bracket[ret], tax[ret]);
    }

}
