#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Nameval {
    char *name;
    int  value;
    struct _Nameval *left;  // lesser
    struct _Nameval *right; // greater
} Nameval;

Nameval *newitem(char *name, int value)
{
    Nameval *newp;
    newp = (Nameval *) malloc(sizeof(Nameval));
    newp->name = name;
    newp->value = value;
    newp->left = NULL;
    newp->right = NULL;
    return newp;
}

Nameval *insert(Nameval *treep, Nameval *newp)
{
    return treep;
}

Nameval *lookup(Nameval *treep, char *name)
{
    return NULL;
}

void printnv(Nameval *p, void *arg)
{
    char *fmt;
    fmt = (char *) arg;
    printf(fmt, p->name, p->value);
}

void applyinorder(Nameval *treep, void (*fn)(Nameval*, void*), void *arg)
{
}

void freeall(Nameval *treep)
{
}

int main()
{
    char *nms[] = { "fred", "wilma", "barney", "betty" };

    Nameval *nvtree = newitem("", 0xdeadbeef);
    Nameval *nval;
    for (int i = 0; i < 4; i++) {
        //addfront(nvlist, newitem(nms[i], i));
        insert(nvtree, newitem(nms[i], i));
    }
 }
