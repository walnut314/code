#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int hindex = 0;
int entries = 0;
int a[] = {1,4,1,4,2,1,3,5,6};
int sz = 9;

void hindexer()
{
    std::vector<int> v(a, a+9);
    std::sort (v.begin(), v.end());  
    int max = *max_element(v.begin(), v.end());
    printf("max: %d\n", max);

    for (int i = 1; i <= max; ++i) {
        entries = 0;
        for (int j = 0; j < sz; j++) {
            if (v[j] >= i) entries++;
        }
        if (entries > i) hindex++;
    }
    printf("hindex: %d\n", hindex);
}

int main()
{
    hindexer();
}
