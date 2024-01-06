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

Nameval *lookup(Nameval *listp, char *name)
{
    for ( ; listp != NULL; listp = listp->next)
        if (strcmp(name, listp->name) == 0)
            return listp;
    return NULL;
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


int main()
{
    char *nms[] = { "fred", "wilma", "barney", "betty" };

    Nameval *nvlist = newitem("", 0xdeadbeef);
    Nameval *nval;
    for (int i = 0; i < 4; i++) {
        nval = newitem(nms[i], i);
        addfront(nvlist, nval);
    }
    apply(nvlist, printnv, "dude: %s: %x\n");
}

