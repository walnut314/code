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
    int max = *std::max_element(v.begin(), v.end());
    printf("max %d\n", max);
    for (auto i : v) { cout << i << ", "; }
    cout << endl;

    for (int i = 1; i <= max; ++i) {
        entries = 0;
        for (int j = 0; j < sz; j++) {
            if (v[j] >= i) {
                entries++;
                hcount[i]++;
            }
        }
        if (entries > i) hindex++;
    }
    printf("hindex: ");
    for (int i = 1; i < max; i++) {
        printf("%d, ", hcount[i]);
    }
    cout << endl;
}

int main()
{
    hindexer();
}

