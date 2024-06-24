#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct _Nameval {
    const char *name;
    int  value;
    struct _Nameval *left;  // lesser
    struct _Nameval *right; // greater
} Nameval;

Nameval *newitem(const char *name, int value)
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
    if (treep == NULL) return newp;
    int cmp = strcmp(newp->name, treep->name);
    if (cmp == 0) {
        printf("dup\n");
    }
    else if (cmp < 0) {
        treep->left = insert(treep->left, newp);
    }
    else {
        treep->right = insert(treep->right, newp);
    }
    return treep;
}

Nameval *lookup(Nameval *treep, const char *name)
{
    if (treep == NULL) return NULL;
    int cmp = strcmp(name, treep->name);
    if (cmp == 0) {
        return treep;
    }
    else if (cmp < 0) {
        return lookup(treep->left, name);
    }
    else {
        return lookup(treep->right, name);
    }
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
    if (treep == NULL) return;
    applyinorder(treep->left, fn, arg);
    (*fn)(treep, arg);
    applyinorder(treep->right, fn, arg);
}

void freeall(Nameval *treep)
{
    if (treep == NULL) return;
    freeall(treep->left);
    freeall(treep->right);
    printf("free: %s\n", treep->name);
    free(treep);
}

int main()
{
    const char *nms[] = { "fred", "wilma", "barney", "betty" };
    int len = sizeof(nms)/sizeof(nms[0]);

    printf("num of entries: %d\n", len);

    Nameval *nvtree = newitem((const char *) "dude", 0xdeadbeef);
    Nameval *nval;
    for (int i = 0; i < len; i++) {
        //addfront(nvlist, newitem(nms[i], i));
        insert(nvtree, newitem(nms[i], i));
    }
    Nameval *s = lookup(nvtree, (const char *) "dude");
    if (s) printf("sea: %s -> %x\n", s->name, s->value);
    s = lookup(nvtree, (const char *) "barney");
    if (s) printf("sea: %s -> %x\n", s->name, s->value);

    applyinorder(nvtree, printnv, (void *) "dude: %s \t- %x\n");
    freeall(nvtree);
 }
