#include <bits/stdc++.h>

using namespace std;

vector<int> subset;
void fsubset(int k, int n)
// k is an element, n is the # of elements in subset
{
    if (k == n+1) {
        printf("{");
        for (auto v : subset) {
            printf("%d, ", v);
        }
        printf("}\n");
    } else {
        subset.push_back(k);
        fsubset(k+1, n);
        subset.pop_back();
        // don't include k in the subset
        fsubset(k+1, n);
    }
}

void stl_sorts()
{
    vector<pair<int,int>> v1;
    v1.push_back({1,5});
    v1.push_back({2,3});
    v1.push_back({1,2});
    sort(v1.begin(),v1.end());
    for (auto v : v1) cout << v.first << ", " << v.second << endl;
    vector<tuple<int,int,int>> v2;
    v2.push_back({2,1,4});
    v2.push_back({1,5,3});
    v2.push_back({2,1,3});
    v2.push_back({3,1,4});
    sort(v2.begin(),v2.end());
    for (auto v : v2) {
        int sz = std::tuple_size<decltype(v)>::value;
        int i1, i2, i3;
        std::tie(i1, i2, i3) = v;
        cout << i1 << ", " << i2 << ", " << i3 << endl;
    }
}

void struct_sort()
{
    struct point {
        int x, y;
        bool operator<(const point &p) {
            if (x == p.x) return y < p.y;
            return x < p.x;
        }
    };
    point p[4] = {{3,4},{1,3},{4,5},{1,2}};
    for (auto pt : p) cout << pt.x << "," << pt.y << endl;
    std::sort( p, p+4); //, &custom_sorter);
    for (auto pt : p) cout << pt.x << "," << pt.y << endl;
}

int collatz()
{
    long long n;
    cin >> n;
    while (true) {
        cout << n << " ";
        if (n == 1) break;
        if (n%2 == 0) n >>= 1;
        else n = 3*n + 1;
    }
    cout << "\n";
    return 0;
}

void bubble(vector<int> array)
{
    int n = array.size();
    for (int x : array) cout << x << ", ";
    cout << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n-1; ++j) {
            if (array[j] > array[i]) {
                swap(array[j], array[i]);
            }
        }
    }
    for (int x : array) cout << x << ", ";
    cout << endl;
}

int counter = 0;
void chess(int y)
{
    static bool init = true;
    static int col[64];
    static int diag1[64];
    static int diag2[64];
    static int n = 3;
    if (init) {
        memset(col, 0, sizeof(col));
        memset(diag1, 0, sizeof(diag1));
        memset(diag2, 0, sizeof(diag2));
        init = false;
    }
    if (y == n) {
        counter++;
        return;
    }
    for (int x = 0; x < n; x++) {
        if (col[x] || diag1[x+y] || diag2[x-y+n-1]) continue;
        col[x] = diag1[x+y] = diag2[x-y+n-1] = 1;
        chess(y+1);
        col[x] = diag1[x+y] = diag2[x-y+n-1] = 0;        
    }
}

int bits(int x)
{
    int count = 0;
    printf("clz = %d\n", __builtin_clz(x));
    printf("ctz = %d\n", __builtin_ctz(x));
    printf("cnt = %d\n", __builtin_popcount(x));
    printf("par = %d\n", __builtin_parity(x));
    while (x) {
        x &= (x-1);
        count++;
    }
    return count;
}

int main()
{
    //collatz();]
    //fsubset(1, 3);
    //chess(0); printf("count: %d\n", counter);
    //printf("bits in: %d\n", bits(5328));
    //vector<int> array{1,3,8,2,9,2,5,6};
    //bubble(array);
    //stl_sorts();
    struct_sort();
    return 0;
}

