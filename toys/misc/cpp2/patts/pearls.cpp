#include <bits/stdc++.h>

using namespace std;

vector<int> arr = {0,1,2,3,4,5,7,8,9};
int N = 10;
void findit()
{
    int n = N/2;
    int sz = arr.size();
    printf("N=%d, N/2=%d, sz=%d\n", N, N/2, sz);
}

#define RIGHT (0)
#define LEFT  (1)
void rotate(int a, int mag, int dir)
{
    int b;
    switch (dir) {
        case RIGHT: {
            b = a >> mag;
            printf("right a:%02x by %d => b:%02x\n", a, mag, b);
            break;
        }
        case LEFT: {
            b = a << mag;
            printf("left a:%02x by %d => b:%02x\n", a, mag, b);
            break;
        }
        default: break;
    }
}


// FIFO -- detect FIFO full
#define BUF_SIZE 8
int FIFO[BUF_SIZE];
int head, tail;
int put(int *FIFO, int x)
{
    // detect fifo full
    FIFO[++head] = x;
    head %= BUF_SIZE;
    return 1;
}
int get(int *FIFO, int *x)
{
    if (head == tail) {
        printf("fifo empty\n");
        return 0;
    }
    *x = FIFO[tail++];
    tail %= BUF_SIZE;
    return 1;
}
int init()
{
    head = tail = 0;
    return 1;
}
void show(int *FIFO)
{
    printf("head: %d, tail: %d\n", head, tail);
    if (head == tail) {
        printf("FIFO empty\n");
        return;
    }
    for (int i = head; i < BUF_SIZE; i++) {
        if (head == i) printf("H ==> ");
        else           printf("      ");
        printf("FIFO[%02d] = %d\n", i, FIFO[i]);
        //if (head == i) printf(" < == head\n");
        //else printf("\n");
    }
    if (tail != head) {
        for (int i = 0; i < head; i++) {
            printf("      FIFO[%02d] = %d", i, FIFO[i]);
            if (tail == i) printf(" <== T\n");
            else printf("\n");
        }
    }
}
void dump(int *FIFO)
{
    for (int i = 0; i < BUF_SIZE; i++) {
        if (head == i) printf("H ==> ");
        else           printf("      ");
        printf("FIFO[%02d] = %d", i, FIFO[i]);
        if (tail == i) printf(" <== T\n");
        else printf("\n");
    }
    printf("\n");
}
void test_fifo()
{
    init();
    dump(FIFO);
    for (int i = 0; i < 4; i++) put(FIFO, i+42);
    printf("post-put 4: h=%d, t=%d\n", head, tail);
    dump(FIFO);
    int x;
    //get(FIFO, &x);
    //printf("x: %d\n", x);
    //dump(FIFO);
    for (int i = 0; i < 4; i++) put(FIFO, i+72);
    printf("post-put 4: h=%d, t=%d\n", head, tail);
    dump(FIFO);
    //get(FIFO, &x);
    //printf("x: %d\n", x);
    //dump(FIFO);
}

#define M 10000000
void intsort()
{
    vector<int> arr = {1,5,2,8,2,3};
    std::bitset<M> b;
    for (auto i : arr) {
        b[i] = 1;
    }
    for (int i = 0; i < M; ++i) {
        if (b[i]) cout << i << endl;
    }
}

void setsort()
{
    set<int> S = {1,5,2,8,2,3};
    set<int>::iterator j;
    for (j = S.begin(); j!=S.end(); ++j)
        std::cout << ' ' << *j;
}

#define SET(s, x) (s |=  (1 << x))
#define CLR(s, x) (s &= ~(1 << x))
#define TST(s, x) (s &   (1 << x))
void myset(int *S, int x)
{
    SET(*S, x);
    SET(*S, 1);
    SET(*S, 0);
    printf("S: %x\n", *S);
    CLR(*S, 1);
    printf("S: %x\n", *S);
}

void rotate()
{
    char arr[] = "abcdefgh";
    int sz, left, right;
    sz = strlen(arr);
    left = 3;
    printf("arr: %s\n", arr);
    char tmp[] = "       ";
    char t;
    for (int i = 0; i < left; i++) {
        //printf("[%d] = %c\n", i, arr[i]);
        printf("swap %d w/%d\n", i, sz-left+i);
        t = arr[i];
        arr[i] = arr[sz-left+i];
        arr[sz-left+i] = t;
    }
    printf("arr: %s\n", arr);
}

#define s binsea
#define MAXN (256)
typedef int DataType;
int tt              = 1;
DataType x[MAXN]; // = {1, 3, 5, 6, 7, 8, 9, 10};
int n; // = MAXN;
int sorted() {
    for (int i = 0; i < n-1; i++)
        if (x[i] > x[i+1]) return 0;
    return 1;
}
int binsea(int t)
{
    int l=0, u=n-1, m;
    int size = n+1, oldsize;
    assert(sorted());
    while (l <= u) {
        assert(l <= u);
        oldsize = size;
        size = u - l + 1;
        assert(size < oldsize);
        //printf("size: %d\n", size);
        m = (l+u)/2;
        if (x[m] < t)
            l = m+1;
        else if (x[m] == t) {
            assert(0 <= m && m < n && x[m] == t);
            return m;
        }
        else
            u = m-1;
    }
    assert((u < 0 || x[u] < t) && (u+1 >= n || x[u+1] > t));
    return -1;
}

void Driver()
{
#if 0
    int ttt;
    while (scanf("%d %d", &n, &ttt) != EOF) {
        for (int i = 0; i < n; i++)
            x[i] = 10*i;
        //printf("%d @ %d\n", ttt, s(ttt));
        for (int i = 0; i < n; i++) {
            printf("%d = %d\n", s(10*i), i);
            //assert(s(10*i)     ==  i);
            //assert(s(10*i - 5) == -1);
        }
    }
#endif
#if 1
    // automate testing
    for (n = 0; n < MAXN; n++) { // be careful not to shadow "n"
        printf("n=%03d - part 1 +", n);
        for (int i = 0; i <= n; i++) {
            x[i] = 10*i;
        }
        clock_t starttime = clock();
        for (int i = 0; i < n; i++) {
            //printf("%d = %d\n", s(10*i), i);
            assert(s(10*i)     ==  i);
            assert(s(10*i - 5) == -1);
        }
        assert(s(10*n - 5) == -1);
        assert(s(10*n)     == -1);
        /* part 2 */
        printf(" part 2 :", n);
        for (int i = 0; i <= n; i++) {
            x[i] = 10;
        }
        if (n == 0)
            assert(s(10) == -1);
        else
            assert(0 <= s(10) && s(10) < n);
        assert(s(5) == -1);
        assert(s(15) == -1);
        clock_t clicks = clock() - starttime;
        double total = (float) (1.0 * clicks/CLOCKS_PER_SEC);
        printf(" t:%lf secs\n", total);
    }

#endif
}

void myslice(vector<int> &v, int m, int n)
{
    int k = n - m + 1;
    auto it = v.cbegin() + m;
    while (it != v.cend() && k--) {
        it = v.erase(it);
    }
}
void dumparr(vector<int> arr) { for (auto i : arr) printf("%d\n", i); printf("\n"); }
int binseaR(vector<int> arr, int t, int n)
{
    int l=0, u=n-1, m, p = -1;
    printf("arr sz: %d\n", n);
    dumparr(arr);
    if (l > u)        {printf("x\n"); return -1;}
    m = (l+u)/2;
    if (arr[m] == t)  {printf("found\n"); p = m; return m;}
    if (t < arr[m])   {printf("rt "); u = m-1; myslice(arr, m, n-1);}
    else              {printf("lft"); l = m+1; myslice(arr, 0, m);}
    binseaR(arr, t, arr.size()); 
    return p;
}

int maxsofar = 0;
int maxendinghere = 0;
int max_subset_sum(vector<int> arr)
{
    int sum = 0;
    vector<int> sum2(arr.size());
    for (int i = 0; i < arr.size(); i++) {
        maxendinghere = max(maxendinghere + arr[i], 0);
        maxsofar = max(maxsofar, maxendinghere);
        sum += arr[i];
        sum2[i] = sum;
        printf("sofar: %d, ending: %d\n", maxsofar, maxendinghere);
    }
    for (int i = 0; i < sum2.size(); i++) {
        printf("sum2[% 03d] = % 03d\n", i, sum2[i]);
    }
    return sum;
}

vector<vector<float>> LatLon = {{12,34},{45,23},{46,93}};
vector<vector<vector<float>>> Cart(3);
float mark_lat = 44;
float mark_lon = 32;
void LatLon_to_Cart()
{
    /*
     *  R is the approx radius of the earth
     *  6371 km
     *
     *  lat and long are in degrees
     *
     *  x = R * cos(lat) * cos(lon)
     *  y = R * cos(lat) * sin(lon)
     *  z = R * sin(lat)
     *
     *  lat = asin(z / R)
     *  lon = atan2(y, x)
     */
    float x, y, z;
    float xm, ym, zm;
    float R = 6371.3;
    float lat, lon;
    float dx;
    for (auto ll : LatLon) {
        lat = ll[0];
        lon = ll[2];
        x = R * cos(lat) * cos(lon);
        y = R * cos(lat) * sin(lon);
        z = R *sin(lat);
        printf("lat: %f, lon: %f => {%f,%f,%f}\n", lat, lon, x, y, z);
        xm = R * cos(mark_lat) * cos(mark_lon);
        ym = R * cos(mark_lat) * sin(mark_lon);
        zm = R *sin(mark_lat);
        dx = sqrt((x - xm)*(x - xm) + (y - ym)*(y - ym) + (z - zm)*(z - zm));
        printf("mark lat: %f, lon: %f => {%f,%f,%f}\n", mark_lat, mark_lon, xm, ym, zm);
        printf("distance: %f\n", dx);
    }
}

int main()
{
    //intsort();
    //setsort();
    //int S;
    //myset(&S, 5);
    //printf("S = %x\n", S);
    //test_fifo();
    //rotate();
    //findit();
    //rotate(0xC, 3, LEFT); 

    //vector<int> arr = {1, 3, 5, 6, 7, 8, 9, 10};
    //printf("binsea: %d @ %d\n", tt, binsea(tt));
    //printf("binsea: %d @ %d\n", t, binseaR(arr, t, arr.size()));
    //Driver();

    //vector<int> arr = {31, -41, 59, 26, -53, 58, 97, -93, -23, 84};
    //max_subset_sum(arr);

    LatLon_to_Cart();

    return 0;
}

