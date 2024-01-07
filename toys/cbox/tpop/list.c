#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Nameval {
    char *name;
    int  value;
    struct _Nameval *next;
} Nameval;

Nameval *newitem(char *name, int value)
{
    Nameval *newp;
    newp = (Nameval *) malloc(sizeof(Nameval));
    newp->name = name;
    newp->value = value;
    newp->next = NULL;
    return newp;
}

Nameval *addfront(Nameval *listp, Nameval *newp)
{
    newp->next = listp->next;
    listp->next = newp;
    return newp;
}

Nameval *addend(Nameval *listp, Nameval *newp)
{
    Nameval *p;
    if (listp == NULL)
        return newp;
    for (p = listp; p->next != NULL; p = p->next)
        ;
    p->next = newp;
    return listp;
}

Nameval *delitem(Nameval *listp, char *name)
{
    Nameval *p, *prev;
    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            printf("delitem: %s from nvlist\n", p->name);
            if (prev == NULL)
                listp = p->next;
            else
                prev->next = p->next;
            free(p);
            return listp;
        }
        prev = p;
    }
    printf("delitem: %s not in list\n", name);
    return NULL;
}

Nameval *lookup(Nameval *listp, char *name)
{
    for ( ; listp != NULL; listp = listp->next)
        if (strcmp(name, listp->name) == 0)
            return listp;
    return NULL;
}

void freeall(Nameval *listp)
{
    Nameval *next;
    for ( ; listp != NULL; listp = listp->next) {
        next = listp->next;
        free(listp);
    }
}

void apply(Nameval *listp, void (*fn)(Nameval*, void*), void *arg)
{
    for ( ; listp != NULL; listp = listp->next)
        (*fn)(listp, arg);
}

void printnv(Nameval *p, void *arg)
{
    char *fmt;
    fmt = (char *) arg;
    printf(fmt, p->name, p->value);
}

void inccounter(Nameval *p, void *arg)
{
    int *ip;
    ip = (int *) arg;
    (*ip)++;
}

int main()
{
    char *nms[] = { "fred", "wilma", "barney", "betty" };

    Nameval *nvlist = newitem("", 0xdeadbeef);
    Nameval *nval;
    for (int i = 0; i < 4; i++) {
        //addfront(nvlist, newitem(nms[i], i));
        addend(nvlist, newitem(nms[i], i));
    }
    Nameval *nv = lookup(nvlist, "barney");
    if (nv) printf("found: %s %x\n", nv->name, nv->value);
    delitem(nvlist, "barney");
    delitem(nvlist, "barney");
    apply(nvlist, printnv, "dude: %s: %x\n");


    int n;
    n = 0;
    apply(nvlist, inccounter, &n);
    printf("%d elements in nvlist\n", n);
    freeall(nvlist);

}

