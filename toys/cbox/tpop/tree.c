#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct _Tree {
    const char *name;
    int  value;
    struct _Tree *left;  // lesser
    struct _Tree *right; // greater
} Tree;

Tree *newitem(const char *name, int value)
{
    Tree *newp;
    newp = (Tree *) malloc(sizeof(Tree));
    newp->name = name;
    newp->value = value;
    newp->left = NULL;
    newp->right = NULL;
    return newp;
}

Tree *insert(Tree *treep, Tree *newp)
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

Tree *lookup(Tree *treep, const char *name)
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

void printnv(Tree *p, void *arg)
{
    char *fmt;
    fmt = (char *) arg;
    printf(fmt, p->name, p->value);
}

void applyinorder(Tree *treep, void (*fn)(Tree*, void*), void *arg)
{
    if (treep == NULL) return;
    applyinorder(treep->left, fn, arg);
    (*fn)(treep, arg);
    applyinorder(treep->right, fn, arg);
}

void freeall(Tree *treep)
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

    Tree *nvtree = newitem((const char *) "dude", 0xdeadbeef);
    Tree *nval;
    for (int i = 0; i < len; i++) {
        //addfront(nvlist, newitem(nms[i], i));
        insert(nvtree, newitem(nms[i], i));
    }
    Tree *s = lookup(nvtree, (const char *) "dude");
    if (s) printf("sea: %s -> %x\n", s->name, s->value);
    s = lookup(nvtree, (const char *) "barney");
    if (s) printf("sea: %s -> %x\n", s->name, s->value);

    applyinorder(nvtree, printnv, (void *) "dude: %s \t- %x\n");
    freeall(nvtree);
 }
