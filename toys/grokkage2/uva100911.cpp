#include <iostream>
#include <algorithm>
#include <string.h>
#include <math.h>

using namespace std;

int N;
double dist[20][20], memo[1<<16];

double matching(int bit_mask)
{
    if (memo[bit_mask] > -0.5)
        return memo[bit_mask];
    if (bit_mask == (1 << 2 * N) - 1)
        return memo[bit_mask] = 0;

    // start here
    return 0.0;
}


int main()
{
    return 0;
}

