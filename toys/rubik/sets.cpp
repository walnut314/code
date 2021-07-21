//#include <bits/stdc++.h>
//using namespace std;

#include "bits.hpp"

// create a virtual class
class IntSetV {
private:
public:
    enum {SET, ARRAY, BST, LIST, BITVEC, BINS};
    virtual void insert(int t) = 0;
    virtual int size() = 0;
    virtual void report(int *x) = 0;
};

class IntSetSet: public IntSetV {
private:
    set<int> S;
public:
    IntSetSet(int& maxelements, int& maxval) { printf("SET\n"); }
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

class IntSetArray: public IntSetV {
private:
    int n, *x;
public:
    IntSetArray(int maxelements, int maxval) {
        printf("ARRAY\n");
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

class IntSetList: public IntSetV {
private:
    int n;
    struct node {
        int val;
        node *next;
        node(int v, node *p) { val = v; next = p; }
    };
    node *head;
public:
    IntSetList(int maxelements, int maxval) {
        printf("LIST\n"); 
        head = new node(maxval, NULL);
        n = 0;
    }
    void insert(int t) { // insert sorted
        node *prev = head;
        node *curr = head->next;
        while (curr) {
            if (t == curr->val) return;
            if (t < curr->val) break;
            prev = curr;
            curr = curr->next;
        }
        if (prev) {
            node *tmp = prev->next;
            node *p = new node(t, prev->next);
            prev->next = p;
            p->next = tmp;
        }
        n++;
    }
    int size() { return n; }
    void report(int *v) {
        int j = 0;
        for (node *p = head->next; p != NULL; p = p->next) {
            v[j++] = p->val;
        }
    }
};

class IntSetBST: public IntSetV {
private:
    int n, *v, vn;
    struct node {
        int val;
        node *left, *right;
        node(int i) { val = i; left = right = NULL; }
    };
    node *root;
    node *rinsert(node *p, int t) {
        if (p == NULL) {
            p = new node(t);
            n++;
        } else if (t < p->val) {
            p->left = rinsert(p->left, t);
        } else if (t > p->val) {
            p->right = rinsert(p->right, t);
        }
        return p;
    }
    void traverse(node *p) {
        if (p == NULL) return;
        traverse(p->left);
        v[vn++] = p->val;
        traverse(p->right);
    }
public:
    IntSetBST(int maxelements, int maxval) { printf("BST\n"); root = NULL; n = 0; }
    void insert(int t) { root = rinsert(root, t);}
    void report (int *x) { 
        v = x; 
        vn = 0; 
        traverse(root); 
    }
    int size() { return n; }
};

class IntSetBitVec: public IntSetV {
private:
    enum { BITSPERWORD = 32, SHIFT = 5, MASK = 0x1F };
    int n, hi, *x;
    void set(int i)  {        x[i>>SHIFT] |=  (1 << (i & MASK)); }
    void clr(int i)  {        x[i>>SHIFT] &= ~(1 << (i & MASK)); }
    int  test(int i) { return x[i>>SHIFT] &   (1 << (i & MASK)); }
public:
    IntSetBitVec(int maxelements, int maxval) { 
        printf("BitVec\n"); 
        hi = maxval;
        x = new int[1 + hi/BITSPERWORD];
        for (int i = 0; i < hi; i++) clr(i);
        n = 0;
    }
    void insert(int t) {
        if (test(t)) return;
        set(t);
        n++; 
    }
    void report (int *v) {
        int j = 0;
        for (int i = 0; i < hi; i++) {
            if (test(i)) v[j++] = i;
        }
    }
    int size() { return n; }
};

class IntSetBins: public IntSetV {
private:
    int n, bins, maxval;
    struct node {
        int val;
        node *next;
        node(int v, node *p) { val = v; next = p; }
    };
    node **bin, *sentinel;
    node *rinsert(node *p, int t) {
        if (p->val < t) p->next = rinsert(p->next, t);
        else if (p->val > t) p = new node(t, p);
        n++;
        return p;
    }
public:
    IntSetBins(int maxelements, int pmaxval) {
        bins = maxelements;
        maxval = pmaxval;
        bin = new node*[bins];
        sentinel = new node(maxval, 0);
        for (int i = 0; i < bins; i++) bin[i] = sentinel;
        n = 0;
    }
    void insert(int t) {
        int i = t / (1 + maxval/bins);
        bin[i] = rinsert(bin[i], t);
    }
    void report (int *v) {
        int j = 0;
        for (int i = 0; i < bins; i++) {
            for (node *p = bin[i]; p != sentinel; p = p->next) {
                v[j++] = p->val;
            }
        }
    }
    int size() { return n; }
};

class IntSetFactory {
private:
    shared_ptr<IntSetV> si = nullptr;
public:
    IntSetFactory(int type, int maxelements, int maxval) {
        switch (type) {
            case IntSetV::SET:   si = make_shared<IntSetSet>(maxelements, maxval); break;
            case IntSetV::BST:   si = make_shared<IntSetBST>(maxelements, maxval); break;
            case IntSetV::LIST:  si = make_shared<IntSetList>(maxelements, maxval); break;
            case IntSetV::ARRAY: si = make_shared<IntSetArray>(maxelements, maxval); break;
        }
    }
    void insert(int t) { si->insert(t); }
    int size() { return si->size(); }
    void report(int *x) { si->report(x); }
};

template <typename T>
class IntSetFactory2 {
private:
    shared_ptr<IntSetV> si = nullptr;
public:
    IntSetFactory2(int maxelements, int maxval) {
        si = make_shared<T>(maxelements, maxval);
    }
    void insert(int t) { si->insert(t); }
    int size() { return si->size(); }
    void report(int *x) { si->report(x); }
};

// this is the best/simplest
template <typename T>
shared_ptr<T> factory2(int maxelements, int maxval) { return make_shared<T>(maxelements, maxval); }

void gensets(int m, int maxval)
{
    int *v = new int[m];

    // all these are good
    //shared_ptr<IntSetV> s = factory2<IntSetSet>(m, maxval); // the simplest

    //IntSetFactory *s = new IntSetFactory(IntSetV::LIST, m, maxval);
    //shared_ptr<IntSetFactory> s = make_shared<IntSetFactory>(IntSetV::LIST, m, maxval);
    //shared_ptr<IntSetV> s = make_shared<IntSetSet>(m, maxval);
    //shared_ptr<IntSetFactory2<IntSetSet>> s = make_shared<IntSetFactory2<IntSetSet>>(m, maxval);
    //shared_ptr<IntSetFactory2<IntSetBST>> s = make_shared<IntSetFactory2<IntSetBST>>(m, maxval);
    
    // this one is broken
    //shared_ptr<IntSetFactory2> s = make_shared<IntSetFactory2<IntSetSet>>(m, maxval);
    
    
    //shared_ptr<IntSetV> s = factory2<IntSetBitVec>(m, maxval); // the simplest
    shared_ptr<IntSetV> s = factory2<IntSetBins>(m, maxval); // the simplest

    while (s->size() < m) { s->insert(rand() % maxval); }
    s->report(v);
    for (int i = 0; i < m; i++) { cout << v[i] << "\n"; }
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));    
    gensets(4, 209);
    return 0;
}

