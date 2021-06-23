#include <bits/stdc++.h>
using namespace std;

class IntSetInt {
private:
    set<int> S;
public:
    IntSetInt(int maxelements, int maxval) {}
    int size() { return S.size(); }
    void insert(int t) { S.insert(t); }
    void report(int *v) {
        int j = 0;
        set<int>::iterator i;
        for (i = S.begin(); i != S.end(); ++i) {
            v[j++] = *i;
        }
    }
};

class IntSetArray {
private:
    int n, *x;
public:
    IntSetArray(int maxelements, int maxval) {
        x = new int[1 + maxelements];
        n = 0;
        x[0] = maxval;
    }
    void insert(int t) {
        int i;
        for (i = 0; x[i] < t; i++) ; // i at next insert
        if (x[i] == t) return;
        for (int j = n; j >= i; j--) {
            x[j+1] = x[j];
        }
        x[i] = t;
        n++;
    }
    int size() { return n; }
    void report(int *v) {
        for (int i = 0; i < n; i++) {
            v[i] = x[i];
        }
    }
};

void gensets(int m, int maxval)
{
    int *v = new int[m];
    IntSetArray S(m, maxval);
    while (S.size() < m) {
        S.insert(rand() % maxval);
    }
    S.report(v);
    for (int i = 0; i < m; i++) {
        cout << v[i] << "\n";
    }
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));    
    gensets(4, 209);
    return 0;
}

