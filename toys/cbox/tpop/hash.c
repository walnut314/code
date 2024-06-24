#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Nameval {
    char *name;
    int  value;
    struct _Nameval *next;
} Nameval;

enum { NHASH = 11, MULTIPLIER = 31 };

Nameval *symtab[NHASH];

unsigned int hash(char *str)
{
    unsigned int h;
    unsigned char *p;
    h = 0;
    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }
    return h % NHASH;
}

Nameval *lookup(char *name, int create, int value)
{
    int h;
    Nameval *sym;

    h = hash(name);
    for (sym = symtab[h]; sym != NULL; sym = sym->next) {
        if (strcmp(name, sym->name) == 0) return sym;
    }
    if (create) {
        sym = (Nameval *) malloc(sizeof(Nameval));
        sym->name = strdup(name);
        sym->value = value;
        sym->next = symtab[h];
    }
    return sym;
}

int main()
{
    char *nms[] = { "fred", "wilma", "barney", "betty" };
    int len = sizeof(nms)/sizeof(nms[0]);

    printf("number of entries: %d\n", len);

    Nameval *nval;
    for (int i = 0; i < len; i++) {
        lookup(nms[i], 1, i);
    }
    for (int i = 0; i < len; i++) {
        if ((nval = lookup(nms[i], 1, i)) != NULL) {
            printf("found: %s \t %x\n", nval->name, nval->value);
        }
    }


}


