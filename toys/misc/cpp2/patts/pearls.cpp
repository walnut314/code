#include <bits/stdc++.h>

using namespace std;

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

int main()
{
    //intsort();
    //setsort();
    //int S;
    //myset(&S, 5);
    //printf("S = %x\n", S);
    //test_fifo();
    rotate();

    return 0;
}

