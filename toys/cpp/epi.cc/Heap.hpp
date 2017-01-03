#ifndef _Heap_h_
#define _Heap_h_

#include "pch.hpp"

using namespace std;

#define PARENT(x)       (x/2)
#define LEFT_CHILD(x)   ((2*(x))+1)
#define RIGHT_CHILD(x)  ((2*(x)))

template <typename T>
struct Heap {
    T *A; // index 0 is not used
    int length;
    int size;
};

template <typename T>
void Heap_tree(Heap<T> *h);

template <typename T>
Heap<T> *Heap_create(int max)
{
    Heap<T> *h = new Heap<T>();
    h->A = (T *) calloc(max, sizeof(T));
    h->size = max;
    h->length = 1;
    return h;
}

template <typename T>
void Heap_insert(Heap<T> *h, T key)
{
    int i;
    T tmp;
    h->A[h->length] = key;
    for (i = h->length; i > 1; --i) {
        if (h->A[i] > h->A[PARENT(i)]) {
            tmp = h->A[i];
            h->A[i] = h->A[PARENT(i)];
            h->A[PARENT(i)] = tmp;
        }
    }
    h->length++;
}

template <typename T>
int Heap_top(Heap<T> *h)
{
    T key = h->A[1], tmp;
    int i;
    h->A[1] = h->A[h->length-1];
    h->length--;
    for (i = 1; i < h->length; ++i) {
        if (h->A[i] < h->A[LEFT_CHILD(i)]) {
            tmp = h->A[i];
            h->A[i] = h->A[LEFT_CHILD(i)];
            h->A[LEFT_CHILD(i)] = tmp;
        } 
        if (h->A[i] < h->A[RIGHT_CHILD(i)]) {
            tmp = h->A[i];
            h->A[i] = h->A[RIGHT_CHILD(i)];
            h->A[RIGHT_CHILD(i)] = tmp;
        }
    }
    return key;
}

template <typename T>
void Heap_extract(Heap<T> *h, T key)
{
    int i, j;
    T tmp;
    //cout << "extracting: " << key << endl;
    for (i = 1; i < h->length; ++i) {
        if (key == h->A[i]) {
            h->length--;
            if (i == h->length) {
                h->A[i] = 0;
                return;
            }
            h->A[i] = h->A[h->length];
            for (j = i; j < h->length; ++j) {
                if (h->A[j] < h->A[LEFT_CHILD(j)]) {
                    tmp = h->A[j];
                    h->A[j] = h->A[LEFT_CHILD(j)];
                    h->A[LEFT_CHILD(j)] = tmp;
                } 
                if (h->A[j] < h->A[RIGHT_CHILD(j)]) {
                    tmp = h->A[j];
                    h->A[j] = h->A[RIGHT_CHILD(j)];
                    h->A[RIGHT_CHILD(j)] = tmp;
                }
            }
            return;
        }
    }
}

template <typename T>
void Heap_dump(Heap<T> *h)
{
    int i;
    for (i = 1; i <= h->length-1; ++i) {
        cout << "A[" << i << "] = " << h->A[i] << endl;
    }
}

template <typename T>
void Heap_tree(Heap<T> *h)
{
    int i, level, j, k;
    int lvl[] = {0, 1,  2,  2,  4,  4,  4,  4, \
                    8,  8,  8,  8,  8,  8,  8,  8, \
                   16, 16, 16, 16, 16, 16, 16, 16, \
                   16, 16, 16, 16, 16, 16, 16, 16, 16};
    for (i = 1; i <= h->length-1; ) {
        level = lvl[i];
        switch (level) {
            case 1: cout << "        "; break;
            case 2: cout << "      "; break;
            case 4: cout << "  "; break;
        }
        for (j = i, k = 1; (k <= level) && (i <= h->length-1); ++k, ++j) {
            cout << "[" << h->A[j] << "]";
            ++i;
        }
        cout << endl;
    }
}

#endif // _Heap_h_
