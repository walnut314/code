#include <bits/stdc++.h>

using namespace std;

int hindexer(vector<int> v)
{
    int hindex = 0;
    std::sort(v.begin(), v.end());
    int max = *std::max_element(v.begin(), v.end());
    int sz = v.size();
    vector<int> hcount(sz, 0);
    for (int i = 1; i <= max; ++i) {
        for (int j = 0; j < sz; j++) {
            if (v[j] >= i) {
                //hcount[i]++;
                // since the array is sorted, instead of ++, 
                // add the length of subarray.
                int sub_sz = sz - j;
                hcount[i] = sub_sz;
                break;
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
    return hindex;
}

int main()
{
    int a[] = {1,4,1,4,2,1,3,5,6};
    std::vector<int> v(a, a+9);
    hindexer(v);
}

