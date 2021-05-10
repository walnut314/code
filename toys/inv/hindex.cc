#include <bits/stdc++.h>

using namespace std;

int hindex = 0;
int entries = 0;
int a[] = {1,4,1,4,2,1,3,5,6};
int sz = 9;
int hcount[9];

void hindexer()
{
    std::vector<int> v(a, a+9);
    std::sort(v.begin(), v.end());
    int max = *std::max_element(v.begin(), v.end());
    for (int i = 1; i <= max; ++i) {
        for (int j = 0; j < sz; j++) {
            if (v[j] >= i) {
                hcount[i]++;
            }
        }
        printf("hcount[%d] = %d\n", i, hcount[i]);
        if (hcount[i] < i) {
            printf("!!! %d < %d so hindex = count-1 (%d)\n", hcount[i], i, i-1);
            hindex = i - 1;
            break;
        }
    }
    printf("hindex: %d\n", hindex);
}

int main()
{
    hindexer();
}

